#include <stdio.h> // I/O
#include <stdlib.h> // Malloc calloc
#include <string.h> // strncpy, strlen
#include <ctype.h> // isalpha

#include "Hw2.h"

char * getSubstring(char * content, int left, int right)
{
    int len = strlen(content);
    int subLength = right - left + 1;
    char * subStr = (char*)malloc((subLength + 1) * sizeof(char));

    strncpy(subStr, content+left, subLength);
    subStr[subLength] = '\0';

    return subStr;
}

int isValid(char s)
{
    if (isalpha(s) || isdigit(s) || s == '&') return 1;
    else return 0;
}

// Splits a string into arguments for a shell
// Does not account the spaces between quotes ("") or symbols
// Has a set buffer of 32 chars per argument
int makearg(char *s, char ***args) 
{
    int i = 0;
    int left = 0;
    int right = 0;

    int validCount = 0;
    const int bufferLength = 256;
    int number_of_strings = 0;

    // Finds number_of_strings
    for (int i = 0; i < strlen(s); i++)
    {
        // Accounts for a string with a single argument having no delimiters
        if ((isValid(s[i]) != 0 || s[i] == '-') && validCount == 0) 
        {
            validCount++;
            number_of_strings++;
            continue;
        }
        // We passed a space and look for next argument
        if (s[i] == ' ' || s[i] == '=') 
        {
            // if s[i-1]  is valid and s[i+1] is valid alpha characters with the = we add a string
            // segmentation fault without the check.
            if (isValid(s[i-1]) && isValid(s[i+1]) && s[i] == '=') number_of_strings++;
            validCount = 0;
            continue;
        }

    }

    // Allocate dynamic memory for a bunch of strings in a char ** array
    // Plus 1 to account for NULL
    char **array = (char**)calloc(sizeof(char*), number_of_strings+1); 
    char *arg_string = (char*)calloc(sizeof(char), bufferLength);
    
    i = 0;
    validCount = 0;
    // Splits passed in string into multiple tokens
    while (right < strlen(s)+1) // +1 for NULL character
    {
	// Account for spaces being left in front of argument.
        if ((s[left] == ' ' && (isValid(s[left+1]) || s[left+1] == '-')))
        {
            left++;
        }
        // Account for excess spaces
        if ((s[right] == ' ' && s[right+1] == ' ') && validCount == 0) 
        {
            right++;
            left = right;
            continue;
        }
        // Keep track of alpha characters
        if (isValid(s[right]) || s[right] == '-')
        {
            validCount++;
            right++;
            continue;
        }
        // OGet shell variables
        if (s[right] == '=' && validCount > 0)
        {
            // Here we would wanna check whether we are in an option or not such as gcc -name-search-dirs=cc1
            // Get substring of current argument and set it to array
            if (left < right-1)
            {
                arg_string = getSubstring(s, left, right-1);
                array[i] = arg_string;
                i++;
            }

            arg_string = getSubstring(s, right, right);
            array[i] = arg_string;
            
            i++;
            validCount = 0;
            right++;
            left = right;

            // Account for specific case of format abc=x (single char expression)
            if (s[right+1] == '\0')
            {
                if (isValid(s[right]))
                {
                    arg_string = getSubstring(s, right, right);
                    array[i] = arg_string;
                    break;
                }
            }
        }
        if ((s[right] == ' ' || s[right] == '\0') && validCount > 0)
        {
            // Get substring of current argument and set it to array
            arg_string = getSubstring(s, left, right-1);
            array[i] = arg_string;

            i++;
            validCount = 0;
            if (s[right] == '\0') break;
            right++;
            left = right;
        }
        else 
        {
            right++;
        }
    }

    // Resolve
    *args = array;
    if (number_of_strings == 0) return -1;
    return number_of_strings; // number of arguments
}

// int main()
// {
//     char ** argv; 
//     int argc = 0;

//     char * input_contents = malloc (sizeof(char) * 256);

//     printf("Input: ");
//     while (fgets(input_contents, 256, stdin) != NULL)
//     {
//         // Gets rid of \n character
//         int length = strlen(input_contents);
//         if (length > 0 && input_contents[length-1] == '\n')
//         {
//             input_contents[length - 1 ] = '\0';
//         }

//         argc = makearg(input_contents, &argv);

//         printf("Number of arguments: %d\n", argc);
//         for (int i = 0; i < argc; i++)
//         {
//             printf("%s\n", argv[i]);
//         }
//         printf("Input: ");
//     }

//     return 0;
// }
