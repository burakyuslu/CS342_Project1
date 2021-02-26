#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>

// constants for mode
const int MODE_NORMAL = 1;
const int MODE_TAPPED = 2;

/** 
 * Returns whether the entered string is a compound command or not.
 * Commands containing | are compound commands.
 * Retuns true if it indeed is a compound command. False otherwise.
 */
bool isCompoundCommand( char inputStr[] ){
    if( strchr( inputStr, '|') != NULL){
        return true;
    }
    else {
        return false;
    }
}

void parseBasicCommand(){

}

void parseCompoundCommand(){

}

void execBasicComm(){

}

void execCompCommNormal(){

}

void execCompCommTapped(){

}


/**
 * Takes 2 command line arguments, N and mode. N is the number of bytes to read / write in mode 2.
 * mode is the selection between normal or tapped mode.
 * Normal mode is 1, tapped mode is 2
 * Intercepting shell program.
 * Takes commands from the user as strings and executes them.
 * Runs in 2 different modes: Normal mode or tapped mode.
 * Creates child processes to run the command. Also runs compund commands.
 * In normal mode a single Linux pipe between childs with redirection is used for interprocess communication.
 * In tapped mode 2 Linux pipes to main are used for communication. In this case the data flow is indirect.
 * Parent process reads the data coming from one of the childs and writes that data to the other child.
 * 
 */
int main(int argc, char *argv[])
{
    // check if the program was called correctly
    if( argc != 3){
        printf( "Program call was performed incorrectly.\n");
        printf( "Program needs to be called as \"./isp N mode\" where N and mode are integer values.\n");
        printf( "Exiting program...\n");
        return -1;
    }

    int n = atoi( argv[1]);
    int mode = atoi( argv[2]);

    // check that mode value is valid
    if( mode != 1 && mode != 2){
        printf("Mode value can be 1 or 2.\n");
        printf("Exiting program...\n");
    }
    else{
        printf( "Initializing the intercepting shell program...\n");
        printf( "Program will wait for your input, and execute it.\n");
        printf( "You can exit anytime by entering \"exit\".\n");
    }

    if( mode == MODE_NORMAL){
        char inputLine[1024];
        
        // take inputs from user as long as he did not initiate to exit
        while( true){
            printf( "isp$ ");
            fgets(inputLine, 1024, stdin);
            inputLine[strcspn(inputLine, "\n")] = '\0';
            printf("\n");


            if( strcmp( inputLine, "exit") == 0){
                printf( "DEBUG: Exit entered\n");
                break;
            }

            bool flagCompoundCommand = isCompoundCommand(inputLine);

            // execute the basic command
            if( flagCompoundCommand == false){
                printf("DEBUG: basic\n");

            }

            // execute the compound command
            else if( flagCompoundCommand == true){
                printf("DEBUG: compound\n");
            }

        }

        printf( "Exiting program...");
        return 0;
    }

    else if( mode == MODE_TAPPED){

    }

    return 0;
}
