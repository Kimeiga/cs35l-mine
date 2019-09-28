//
// Created by Kimeiga on 5/8/2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void tr2u(char *from, const char *to){

    // set up ascii map
    char asciiChars[256];

    for(int i = 0; i < 256; i++)
    {
        asciiChars[i] = (char) i;
    }

    // add ascii transformations
    for (int j = 0; j < strlen(from); ++j) {
        asciiChars[from[j]] = to[j];
    }

    int current;
    char * currentChar;
    int status = read(0, &current, 1); // EOF = 1

    // write char read char
    while(status > 0)
    {
        currentChar = &asciiChars[current];
        write(1, currentChar, 1);
        status = read(0, &current, 1);
    }

    if(status == -1){
        fprintf(stderr, "Error reading file.");
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    // Check number of arguments
    if (argc != 3)
    {
        fprintf(stderr, "Requires 2 arguments.");
        exit(1);
    }

    // Check length of inputs
    char* from = argv[1];
    char* to = argv[2];


    // Checks "from" for duplicates
    for (int i = 0; i < strlen(from); i++)
    {
        for (int j = i + 1; j < strlen(from); j++)
        {
            if (from[i] == from[j])
            {
                fprintf(stderr, "From should have unique characters");
                exit(1);
            }
        }
    }

    if (strlen(from) != strlen(to))
    {
        fprintf(stderr, "From and To must be the same length.");
        exit(1);
    }

    tr2u(argv[1], argv[2]);
}

