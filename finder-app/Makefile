ifeq ($(CROSS_COMPILE), aarch64-none-linux-gnu-)
	cc = aarch64-none-linux-gnu-gcc
	cxx = aarch64-none-linux-gnu-g++
else
	cc = gcc
	cxx = g++
endif

writer: writer.c      
	$(cc) -o writer writer.c -I.

clean:
	rm -f writer