#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>

int main(int argc, char* argv[]){
openlog(NULL,0,LOG_USER);
int retval = 0;
    if(argc == 3){
        syslog(LOG_DEBUG,"Made it into if statement, Correct number of arguments.");
        FILE *fileptr;
        fileptr = fopen(argv[1], "w");
        if(fileptr!=NULL){
            syslog(LOG_DEBUG,"Successfully opened file");
        }
        else{
            syslog(LOG_ERR, "Could not open file");
        }

        retval = fputs(argv[2], fileptr);
        if(retval != -1){
            syslog(LOG_DEBUG, "Successfully wrote %s to file %s", argv[1], argv[2]);
        }
        else{
            syslog(LOG_ERR, "Could not write to file");
        }

        retval = fclose(fileptr);
        if(retval != -1){
            syslog(LOG_DEBUG, "Successfully closed file");
        }
        else{
            syslog(LOG_ERR, "Could not close file");
        }
    }
    else{
        syslog(LOG_ERR, "Invalid number of arguments: %d", argc);
        retval = 1;
    }
return retval;
}