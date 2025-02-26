/*
CS 240 Hw4
Samuel Beal
Bob Rinker 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    // exec
#include <sys/types.h> // sleep and process ID
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <paths.h>

#include "Hw2.h"
// #include "Hw3.c"

#define max_process_count 5

int status;
const int bufferLength = 256;

char ** shell_variables;
char ** paths;
int current_shell_variables = 0;

pid_t childpid[max_process_count];

int uish_exit();
void uish_launch(char **args, int *current_process_count, int ampersand, int argcTwo);

char *builtin_str[] = {
    // "cd",
    // "help",
    "exit"};

// builtin array for functions like exit.
int (*builtin_func[])(char **) = {
    // &lsh_cd,
    // &lsh_help,
    &uish_exit};

// exit command, returns 0 if called
int uish_exit()
{
    return 0;
}

// Determines if it is a valid char for shell_variables
int valid_char(char *tmp)
{
    for (int i = 0; tmp[i] != '\0'; i++)
    {
        if (isalpha(tmp[i]) || isdigit(tmp[i]) || tmp[i] == '_' || tmp[i] == '\n');
        else
            return 0;
    }
    return 1;
}

// Copys two strings together
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator

    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

// Handles the path finding and execution of execv
void handle_execv(char ** args, int argcTwo)
{
    //execvp(args[0], &args[0]);
    // Get rid of ampersand
    for (int i = 0; i < argcTwo; i++)
    {
        if (strcmp(args[i], "&") == 0) args[i] = NULL;
    }
    
    // Get path and allocate to prepare for searching
    //char * PATH = "/usr/libexec/gcc/x86_64-linux-gnu/13/cc1";
    char * PATH = getenv("PATH");
    //strcat(PATH, oldPATH);
    paths = (char**)calloc(sizeof(char*), 32);
    char * ptr;
    size_t path_length = 0;
    
    // Get a new buffer with current working directory
    char single_buf[512];
    getcwd(single_buf, sizeof(single_buf));
    strcat(single_buf, "/");
    strcat(single_buf, args[0]);
    //printf("current working dir %s\n", single_buf);

    // Couldn't find cc1 with gcc, but tar and everything else works.
    // Test for this or specified directory
    if (!access(single_buf, X_OK))
        execv(single_buf, &args[0]);

    // Makes ./testprog work
    // if (!access(args[0], X_OK))
    //     execv(args[0], &args[0]);

    char buf[512];
    
    int i = 0;
    ptr = strtok(PATH, ":");

    while (ptr != NULL)
    {
        paths[i] = ptr;

        // Create buffer and search through it
        strcpy(buf, paths[i]);
        strcat(buf, "/");
        strcat(buf, args[0]);

        if (!access(buf, X_OK))
            execv(buf, &args[0]);

        ptr = strtok(NULL, ":");
        i++;
    }
}

int handle_shell_variable(char **args, int argcTwo)
{
    // Make sure we have a valid 
    if (argcTwo > 2 && strcmp(args[1], "=") == 0);
    else return 0;

    // Check for first char to meet format
    char *str = args[0];
    if (isalpha(str[0]) || str[0] == '_');
    else
        return 0;

    // Check for the rest of the arguments.
    for (int i = 0; i < argcTwo; i++)
    {
        if (i == 1) continue;
        char *tmp = args[i];
        if (valid_char(tmp));
        else
            return 0;
    }

    // Allocate memory for the left side and right side of equal sign
    // Then put them in our global shell_variable array.
    char *variable = (char*)calloc(sizeof(char), bufferLength);
    variable = args[0];
    shell_variables[current_shell_variables] = variable;

    char *variable2 = (char*)calloc(sizeof(char), bufferLength);
    variable2 = args[2];
    shell_variables[current_shell_variables+1] = variable2;
    current_shell_variables+=2;
    return 1;
}

// Main program loop
int uish_execute() 
{
    // Initalize
    char **args;
    int argcTwo = 0;
    int current_process_count = 0;
    int ampersand = 0;
    char *file_contents = malloc(sizeof(char) * 256);

    // Main loop
    if (fgets(file_contents, 256, stdin) != NULL)
    {
        // Remove newlines
        int length = strlen(file_contents);
        if (length > 0 && file_contents[length - 1] == '\n')
        {
            file_contents[length - 1] = '\0';
        }

        // Tokenize the arguments
        argcTwo = makearg(file_contents, &args);
        args[argcTwo] = NULL;
        if (argcTwo < 1)
            return 1;

        // for (int i = 0; i < argcTwo; i++)
        // {
        //     printf("Argument: %d, %s\n", i, args[i]);
        // }

        // Built in functions like exit
        if (strcmp(args[0], builtin_str[0]) == 0)
            return (*builtin_func[0])(args);

        // Account for echo using shell variables. 
        if (strcmp(args[0], "echo") == 0)
        {
            //printf("Inside echo\n");
            for (int i = 0; i < current_shell_variables; i+=2)
            {
                char* s = concat("$", shell_variables[i]);
                if (strcmp(args[1], s) == 0)
                {
                    // Print out shell variable 
                    printf("%s\n", shell_variables[i+1]);
                    free(s); 
                    return 1;
                }
                free(s);
            }
        }
        if (argcTwo > 1)
        {
            // We check for ampersand and change accordingly
            if (strcmp(args[argcTwo - 1], "&") == 0)
            {
                ampersand = 1;
                uish_launch(args, &current_process_count, ampersand, argcTwo);
                return 1;
            }
        }
        if (handle_shell_variable(args, argcTwo) == 1)
        {
            //printf("We made it to shell\n");
            return 1;
        }
        // Run commands as normal
        uish_launch(args, &current_process_count, ampersand, argcTwo);
    }
    free(file_contents);
    

    return 1;
}

void uish_launch(char **args, int *current_process_count, int ampersand, int argcTwo)
{
    current_process_count++;
    // Checks for empty slot and allows us to continue if zero
    int i = 0;
    for (; i < max_process_count; i++)
    {
        if (childpid[i] == 0)
        {
            break;
        }
    }

    if ((childpid[i] = fork()) == 0)
    {
        handle_execv(args, argcTwo);
        printf("Invalid command\n");
        _exit(1);
    }
    else if (childpid[i] < 0)
    {
        perror("fork error");
    }

    if (ampersand)
    {
        // Background execution
        uish_execute();

        // Wait as normal.
        pid_t exitChild = waitpid(childpid[i], &status, 0); // -1 means any
        *current_process_count--;

        // We change our exitChild's array index to 0 to allow a new process
        int j = 0;
        for (; j < max_process_count; j++)
        {
            if (childpid[j] == exitChild)
            {
                childpid[j] = 0;
            }
        }
        
    }
    else
    {
        // Wait for child as normal
        pid_t exitChild = waitpid(childpid[i], &status, 0); // -1 means any
        *current_process_count--;

        // We change our exitChild's array index to 0 to allow a new process if called
        int j = 0;
        for (; j < max_process_count; j++)
        {
            if (childpid[j] == exitChild)
            {
                childpid[j] = 0;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    // Setup for 32 shell variables
    int max_shell_variables = 64; // 64/2 total
    shell_variables = (char**)calloc(sizeof(char*), max_shell_variables); 
    memset(childpid, 0, max_process_count * sizeof(pid_t));
    int status;
    

    long PID = getpid();

    int state = 1;
    while (state == 1)
    {
        printf("uish$ ");        
        state = uish_execute(); // args, argcTwo, file_contents
    }

    for (int i = 0; i < current_shell_variables; i++)
    {
        free(shell_variables[i]);
    }
    free(shell_variables);
    free(paths);
}