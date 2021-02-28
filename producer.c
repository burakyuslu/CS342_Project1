// Author: Burak Yigit Uslu
// CS 342 Project 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


const char charset[] ="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

int main(int argc, char *argv[])
{
    if( argc != 2){
        printf( "Error, wrong function call!");
        exit(-1);
    }

    int m = atoi(argv[1]);

    srand(time(0));
    char genVal;
    int index;
    for( int i = 0; i < m; i++){
        index = rand() % 61;
        genVal = charset[index];
        printf( "%c", genVal);
    }

}
