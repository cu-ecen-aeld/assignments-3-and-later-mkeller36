#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
HOME=/home/keller/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

#if passed one argument
if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    # deep clean, removes .config file 
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- mrproper
    # build a kernal image for booting with qemu 
    make -j4 ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- all
    # build kernel modules 
    # make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- modules 
    # build device tree 
    # make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- dtbs 
    # end TODO
fi

echo "Adding the Image in outdir"

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
mkdir -p usr/bin usr/lib usr/sbin
mkdir -p var/log
# END TODO

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    make distclean
    make defconfig
else
    cd busybox
fi

# TODO: Make and install busybox
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make CONFIG_PREFIX=linuxroots/assignment3/rootfs ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install
# END TODO

echo "Library dependencies"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs
cp ${FINDER_APP_DIR}/ld-linux-aarch64.so.1 ${OUTDIR}/rootfs/lib
cp ${FINDER_APP_DIR}/libm.so.6 ${OUTDIR}/rootfs/lib64
cp ${FINDER_APP_DIR}/libresolv.so.2 ${OUTDIR}/rootfs/lib64
cp ${FINDER_APP_DIR}/libc.so.6 ${OUTDIR}/rootfs/lib64

# TODO: Make device nodes
cd "${OUTDIR}/rootfs"
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1

# TODO: Clean and build the writer utility
cd ${FINDER_APP_DIR}
make clean
make CROSS_COMPILE=${CROSS_COMPILE}  

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cd ${FINDER_APP_DIR}
cp writer ${OUTDIR}/rootfs/home

cp finder.sh ${OUTDIR}/rootfs/home
cp -r ../conf ${OUTDIR}/rootfs/home
cp finder-test.sh ${OUTDIR}/rootfs/home

cp autorun-qemu.sh ${OUTDIR}/rootfs/home

# TODO: Chown the root directory
cd rootfs/
sudo chown -R root:root * 

# TODO: Create initramfs.cpio.gz
cd "$OUTDIR/rootfs"
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
cd ${OUTDIR}
gzip -f initramfs.cpio
