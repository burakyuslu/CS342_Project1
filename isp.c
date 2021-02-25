#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

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


}
