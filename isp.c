#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

// constants for mode
const int MODE_NORMAL = 1;
const int MODE_TAPPED = 2;

// constants for pipe
const int PIPE_READ_END = 0;
const int PIPE_WRITE_END = 1;

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

/** 
 * Parses a basic (non-compound) command. 
 * The command and arguments are parse as an array into processedStrArr.
 */
void parseBasicCommand( char* rawStr, char** processedStrArr){
    // if not reached the end of the rawStr
    while (*rawStr != '\0') {

        // go to the next word/non-trivial character
        while (*rawStr == '\t' || *rawStr == ' ' ||  *rawStr == '\n'){
            *rawStr = '\0';
            rawStr++;
        }
        
        // save the reached argument to the argument array
        *processedStrArr = rawStr;  
        processedStrArr++;

        // skip the argument
        while (*rawStr != '\0' && *rawStr != ' ' && *rawStr != '\t' && *rawStr != '\n'){
            rawStr++;             
        }
    }

    // marks the end of the argument list
    *processedStrArr = '\0';
}

/** 
 * Parses a compound command. 
 * Returns the command and arguments as arrays.
 * First array is the left command and second array is the right command.
 */
void parseCompoundCommand(char* rawStr, char** processedStrArr1, char** processedStrArr2){
    // process first command
    // if not reached the end of the rawStr
    while (*rawStr != '\0' && *rawStr != '|') {

        // go to the next word/non-trivial character
        while (*rawStr == '\t' || *rawStr == ' ' ||  *rawStr == '\n'){
            *rawStr = '\0';
            rawStr++;
        }
        
        // if reached the end of first command, break
        if( *rawStr == '|'){
            break;
        }

        // save the reached argument to the argument array
        *processedStrArr1 = rawStr;  
        processedStrArr1++;

        // skip the argument
        while (*rawStr != '\0' && *rawStr != ' ' && *rawStr != '\t' && *rawStr != '\n'){
            rawStr++;             
        }
    }

    // marks the end of the argument list
    *processedStrArr1 = '\0';

    rawStr++; // skip the |
    // process second command
    // if not reached the end of the rawStr
    while (*rawStr != '\0') {

        // go to the next word/non-trivial character
        while (*rawStr == '\t' || *rawStr == ' ' ||  *rawStr == '\n'){
            *rawStr = '\0';
            rawStr++;
        }
        
        // save the reached argument to the argument array
        *processedStrArr2 = rawStr;  
        processedStrArr2++;

        // skip the argument
        while (*rawStr != '\0' && *rawStr != ' ' && *rawStr != '\t' && *rawStr != '\n'){
            rawStr++;             
        }
    }

    // marks the end of the argument list
    *processedStrArr2 = '\0';
}

/**
 * Using execvp syscall executes a basic command.
 */
void execBasicComm(char** commArr){

    if( fork() == 0){
        if( execvp(*commArr, commArr) < 0) {
            printf("Basic command execution failed.\n");
        }
        exit(0);
    }
    else {
        // parent waits for child to finish execution
        wait(NULL);
    }

}

/**
 * Executes comppound commands.
 * The data transfer is done through a pipe from first child process to the other.
 * Used while in normal mode.
 */
void execCompCommNormal(char** firstCommArr, char** secondCommArr){

    int fd[2];
    pipe(fd);

    // first child
    if( fork() == 0){
        close(fd[PIPE_READ_END]);
        dup2(fd[PIPE_WRITE_END], 1);

        if( execvp(*firstCommArr, firstCommArr)){
            printf("Execution of the first/left command failed.\n");
        }
        close(fd[PIPE_WRITE_END]);
        exit(0);
    }
    // second child
    else if (fork() == 0){
        close(fd[PIPE_WRITE_END]);
        dup2(fd[PIPE_READ_END], 0);
        if( execvp(*secondCommArr, secondCommArr)){
            printf("Execution of the second/right command failed.\n");
        }
        close(fd[PIPE_READ_END]);
        exit(0);
    }
    // parent waits for child tasks to finish execution
    else{
        wait(NULL);
    }
    close(fd[PIPE_READ_END]);
    close(fd[PIPE_WRITE_END]);
}

/**
 * Executes comppound commands.
 * The data transfer is done through the first child to parent to the second child.
 * Used while in tapped mode.
 * Parent reads and writes n-bytes at a time, where n is an command line argument.
 */
void execCompCommTapped(int nValue, char** firstCommArr, char** secondCommArr, int statChar, int statRead, int statWrite ){
    pid_t parentPid = getpid();
    pid_t child1Pid;
    pid_t child2Pid;

    int fd1[2];
    int fd2[2];

    pipe(fd1);
    pipe(fd2);

    // close(fd1[PIPE_WRITE_END]);
    // close(fd2[PIPE_READ_END]);

    // set the pipe for the first child
    if( fork() == 0){
        child1Pid = getpid();
        // will not use pipe 2
        close(fd2[PIPE_READ_END]);
        close(fd2[PIPE_WRITE_END]);

        // will use write end of pipe 1
        close(fd1[PIPE_READ_END]);
        dup2(fd1[PIPE_WRITE_END], 1);
        close(fd1[PIPE_WRITE_END]);

        if( execvp(*firstCommArr, firstCommArr)){
            printf("Execution of the first/left command failed.\n");
        }
    }
    else {
        // set the pipe for the second child
        if( fork() == 0){
            child2Pid = getpid();

            // will not use pipe 1
            close(fd1[PIPE_READ_END]);
            close(fd1[PIPE_WRITE_END]);

            // will use read end of pipe 2
            close(fd2[PIPE_WRITE_END]);
            dup2(fd2[PIPE_READ_END], 0);
            close(fd2[PIPE_READ_END]);
            if( execvp(*secondCommArr, secondCommArr)){
                printf("Execution of the second/right command failed.\n");
            }
        }
        // set the pipe for the parent
        // else {
            // will use read end of pipe 1
            // close(fd1[PIPE_WRITE_END]);
            // dup2(fd1[PIPE_READ_END], 0);

            // will use the write end of pipe 2
            // close(fd2[PIPE_READ_END]);
            // dup2(fd2[PIPE_WRITE_END], 1);
        // }
    }

    char* buffer[nValue + 1];
    int readByteCnt = 0;
    if( getpid() == parentPid){
        do {
            // read n bytes & write those bytes to second child
            close(fd1[PIPE_WRITE_END]);
            readByteCnt = read( fd1[PIPE_READ_END], buffer, nValue);
            buffer[readByteCnt] = '\0';
            close(fd2[PIPE_READ_END]);
            write( fd2[PIPE_WRITE_END], buffer, readByteCnt);
        } while (readByteCnt > 0);
        close(fd1[PIPE_READ_END]);
        close(fd2[PIPE_WRITE_END]);
        wait(NULL);
        wait(NULL);
    }

    if( getpid() == child1Pid){
        exit(0);
    }
    else if ( getpid() == child2Pid){
        exit(0);
    }
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
                break;
            }

            bool flagCompoundCommand = isCompoundCommand(inputLine);

            // execute the basic command
            if( flagCompoundCommand == false){

                // parse the input into commands & arguments
                char  *commandArr[64];
                parseBasicCommand(inputLine, commandArr);

                // execute the command
                execBasicComm( commandArr);

            }

            // execute the compound command
            else if( flagCompoundCommand == true){

                char *commandArr1[64];
                char *commandArr2[64];

                parseCompoundCommand(inputLine, commandArr1, commandArr2);

                // execute the command
                execCompCommNormal( commandArr1, commandArr2);

            }

        }

        printf( "Exiting program... \n");
        return 0;
    }

    else if( mode == MODE_TAPPED){
        char inputLine[1024];
        
        printf( "isp$ ");
        // take inputs from user as long as he did not initiate to exit
        while( true){
            fgets(inputLine, 1024, stdin);
            inputLine[strcspn(inputLine, "\n")] = '\0';
            printf("\n");

            if( strcmp( inputLine, "exit") == 0){
                break;
            }

            bool flagCompoundCommand = isCompoundCommand(inputLine);

            // execute the basic command
            if( flagCompoundCommand == false){

                // parse the input into commands & arguments
                char  *commandArr[64];
                parseBasicCommand(inputLine, commandArr);

                // execute the command
                execBasicComm( commandArr);
            }

            // execute the compound command
            else if( flagCompoundCommand == true){

                char *commandArr1[64];
                char *commandArr2[64];
                int statCharCnt = 0;
                int statReadCnt = 0;
                int statWriteCnt = 0;

                parseCompoundCommand(inputLine, commandArr1, commandArr2);
                
                // execute the command
                execCompCommTapped( n, commandArr1, commandArr2, statCharCnt, statReadCnt, statWriteCnt);

            }
            printf( "isp$ ");
        }
        printf( "Exiting program... \n");
        return 0;   
    }
    return 0;
}