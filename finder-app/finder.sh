#!/bin/bash
filesdir=$1
searchstr=$2

if [ -z $filesdir ] || [ -z $searchstr ]
then 
    echo "finder: Arguments not specified"
    exit 1
else 
    if [ ! -d $filesdir ] 
    then    
        echo "Directory not found"
        exit 1 
    else 
        files=$( grep -rl $searchstr* $filesdir | wc -l )
        lines=$( grep -rc $searchstr* $filesdir | wc -l )
        printf "The number of files are %d and the number of matching lines are %d" $files $lines
        exit 0
    fi
fi