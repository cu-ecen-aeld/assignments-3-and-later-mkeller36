#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

void main(int argc, char* argv[]){
    if(argc == 3){
        FILE *fileptr;
        char *str;
        str = argv[2];
        fileptr = fopen(argv[1], "w");
        fputs(str, fileptr);
        fclose(fileptr);
    }
    else{
        printf("writer: Arguments not specified");
    }
}