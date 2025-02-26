#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    // exec
#include <sys/types.h> // sleep and process ID
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#include "Hw2.h"

int main(int argc, char *argv[])
{
    // Get number of processes to create
    long processCount = 0;
    if (argc > 1)
    {
        processCount = strtol(argv[argc - 1], NULL, 10);
    }
    else
    {
        printf("Error, missing an argument\n");
        exit(EXIT_SUCCESS);
    }

    // Initalize
    char **args;
    int argcTwo = 0;

    pid_t childpid[processCount];
    memset(childpid, 0, processCount * sizeof(pid_t));
    int status;

    char *file_contents = malloc(sizeof(char) * 256);
    int current_process_count = 0;


    // Get input from redirection on cmd line: <filename
    while (fgets(file_contents, 256, stdin) != NULL)
    {
        // Create processes up to the limit
        int i = 0;
        for (; i < processCount; i++)
        {
            if (childpid[i] == 0)
            {
                break;
            }
        }

        // Remove newlines
        int length = strlen(file_contents);
        if (length > 0 && file_contents[length - 1] == '\n')
        {
            file_contents[length - 1] = '\0';
        }

        // Tokenize the arguments
        argcTwo = makearg(file_contents, &args);
        args[argcTwo] = NULL;

        // Run commands 
        current_process_count++;
        if ((childpid[i] = fork()) == 0)
        {
            execvp(args[0], &args[0]);

            perror("Execvp error");
            _exit(1);
        }
        else if (childpid[i] < 0)
        {
            perror("fork error");
        }

        // Keep track of current_process_count, if we are at the limit we wait
        if (current_process_count == processCount)
        {
            pid_t exitChild = waitpid(-1, &status, 0); // -1 means any
            current_process_count--;

            // We change our exitChild's array index to 0 to allow a new process
            int j = 0;
            for (; j < processCount; j++)
            {
                if (childpid[j] == exitChild)
                {
                    childpid[j] = 0;
                }
            }
        }
    }
}