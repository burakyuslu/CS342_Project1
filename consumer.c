// Author: Burak Yigit Uslu
// CS 342 Project 1

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if( argc != 2){
        printf( "Error, wrong function call!");
        exit(-1);
    }

    int m = atoi(argv[1]);

    char temp = 'a';
    for(int i = 0; i < m; i++){
        scanf("%c", &temp);
    }

}
