#include "systemcalls.h"
#include <syslog.h>
#include <errno.h>
#include <stdlib.h>
/*libraries for open */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/*libraries for execl*/
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>


/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

    int retval = false;
    retval = system(cmd);
    if(retval>-1){retval = true;}
    else{retval = false;}
    return retval;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    openlog(NULL,0,LOG_USER);
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

    int status;
    pid_t pid; 
    pid = fork();
    fflush(stdout);
    if(pid<0){
        return false;
    }
    else if(pid==0){

        if(execv(command[0],command)==-1){
            exit(EXIT_FAILURE);
        }
    }

    if(waitpid(pid,&status,0)==-1){
        return false;
    }

    if (WIFEXITED(status) && !WEXITSTATUS(status)) {
        /*check if exited and the exit status */
        va_end(args);
        return true;
    } else {
        // Command failed
        va_end(args);
        return false;
    }
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    int status;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
    int fd = open("redirected.txt", O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd < 0) { 
        perror("open"); 
        abort(); 
    }

    pid_t kidpid = fork();
    if(kidpid == -1){ 
        perror("fork"); 
        abort();
        return false;
    }
    else if (kidpid == 0){
        if (dup2(fd, STDOUT_FILENO) < 0){ 
            perror("dup2"); 
            abort();
            close(fd);
            exit(EXIT_FAILURE); 
            }
        close(fd);
        execv(command[0],command); perror("execv"); abort();
        exit(EXIT_FAILURE);
    }
    else{
        if (waitpid(kidpid, &status, 0) == -1) {
            perror("waitpid failed");
            va_end(args);
            return false;
        }
        close(fd);
        /* do whatever the parent wants to do. */


        if (WIFEXITED(status) && !WEXITSTATUS(status)) {
            /*check if exited and the exit status */
            va_end(args);
            return true;
        } else {
            // Command failed
            va_end(args);
            return false;
        }
    }
}
