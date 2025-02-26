/*
/ Samuel Beal
/ CS 240 Assignment 1
*/

#include <stdio.h> // I/O
#include <unistd.h> // sleep and process ID
#include <stdlib.h>  // for strtol
#include <sys/types.h> // sleep and process ID

/*
This program should contain a loop which sleeps for the
specified amount of time, then wakes up and prints its name, process ID (PID) and iteration
count to stderr.
*/
int main(int argc, char* argv[])
{
    long sleepArg = 0;
    long iterationCount = 0;

    // Account for no arguments
    if (argc > 2) {
        sleepArg = strtol(argv[argc - 2], NULL, 10);
        iterationCount = strtol(argv[argc - 1], NULL, 10);
    }
    int i = 0;
    while (argc > 2 && i < iterationCount)
    {
        // Example: Executing testprog, process id 28435, iteration number 1
        sleep((sleepArg));
        fprintf(stderr, "Executing %s, ", argv[0]);
        fprintf(stderr, "process ID %ld, ", (long)getpid());
        fprintf(stderr, "iteration number %d\n", i + 1);
        i++;
        // wait, exit, getppid, getpid, fork, exec.
    }
    fprintf(stderr, "%s is now exiting.\n", argv[0]);
}
