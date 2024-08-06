#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>

void main(int argc, char* argv[]){
openlog(NULL,0,LOG_USER);
    if(argc == 3){
        syslog(LOG_DEBUG,"Made it into if statement, Correct number of arguments.");
        FILE *fileptr;
        fileptr = fopen(argv[1], "w");
        fputs(argv[2], fileptr);
        fclose(fileptr);
    }
    else{
        syslog(LOG_ERR, "Invalid number of arguments: %d", argc);
    }
}