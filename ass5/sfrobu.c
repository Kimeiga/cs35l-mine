#include<stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <ctype.h>

int frobcmp(const void* array1, const void* array2)
{
    // Casts char pointers a and b from arrays
    const char * a = *(const char **) array1;
    const char * b = *(const char **) array2;

    for(;; a++, b++) //Loop to look at both arrays
    {
        if(*a == ' ' && *b == ' ') {
            return 0;  //both at end of arrays, equal size
        }
        else if (*a == ' ' || ((*a^42) < (*b^42))) {
            return -1; // end of array a or unfrobnicated a < b
        }
        else if (*b == ' ' || ((*a^42) > (*b^42))) {
            return 1;  // end of array b or unfrobnicated a > b
        }
    }
}


void sfrob(int fileSize, int ignoreCase) {
    char* word = (char*)malloc(fileSize * sizeof(char)); //Holds one word at a time
    int wordIterator = 0;

    char** words = (char**)malloc(0); //Array to hold pointers to words
    int wordsIterator = 0;



    char currentChar;
    int size = fileSize;

    while (read(0, &currentChar, sizeof(char)) > 0)
    {
        // Allocate more memory if needed
        if (wordIterator >= size)
        {
            size++;

            char* temp = realloc(word, size * sizeof(char));
            if(temp != NULL)
            {
                //Make the word equal to the reallocated space
                word = temp;
            }
            else
            {
                free(word);
                fprintf(stderr, "Error reAllocation Memory for chars!");
                exit(1);
            }
        }

        if(currentChar == ' '){

            // Skip consecutive spaces
            if ((wordIterator > 0) && (word[wordIterator - 1] == ' '))
            {
                continue;
            }

            if (wordIterator == 0)
            {
                continue;
            }
        }

        // Add char to word
        word[wordIterator] = currentChar;
        wordIterator++;
    }

    // If input ended without a ending space
    if (word[wordIterator - 1] != ' ')
    {
        // Add space to end
        size++;

        char* temp = realloc(word, size * sizeof(char));
        if(temp != NULL)
        {
            word = temp;
        }
        else
        {
            free(word);
            fprintf(stderr, "Error reAllocation Memory for chars!");
            exit(1);
        }

        word[wordIterator] = ' ';
        wordIterator++;
    }

    // if dashF, toupper the word array
    if(ignoreCase){

        for(int i=0; i<strlen(word); i++){
            word[i] = toupper(word[i]);
        }
    }


    int k = 0;
    int start_curr_word = 0;

    for (; k < wordIterator; k++)
    {
        // End of word
        if (word[k] == ' ')
        {
            // Reallocate space

            char** temp = realloc(words, (wordsIterator + 1) * sizeof(char*));
            if(temp != NULL)
            {
                //Make the word equal to the reallocated space
                words = temp;
            }
            else
            {
                free(words);
                fprintf(stderr, "Error reAllocation Memory for pointers!");
                exit(1);
            }

            // Add complete word to the array
            words[wordsIterator] = word + start_curr_word;
            wordsIterator++;

            // Set start address of next word
            start_curr_word = k + 1;
        }
    }




    // Sort frobnicated words
    qsort(words, (size_t) wordsIterator, sizeof(char*), frobcmp);

    char c;


    // Output to stdout
    for (int i = 0; i < wordsIterator; i++)
    {
        for (int j = 0; words[i][j] != ' '; j++)
        {
            c = words[i][j];
            //write(1, &c, 1);
            if(write(1, &c, 1) < 1)
            {
                fprintf(stderr, "Error while writing!");
                exit(1);
            }
        }
        char space = ' ';
        if(write(1, &space, 1) < 1)
        {
            fprintf(stderr, "Error while writing!");
            exit(1);
        }
    }

    free(word);
    free(words);

    exit(0);
}


int main(int argc, char* argv[])
{
    int ignoreCase = 0;
    // is there -f
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            ignoreCase = 1;
        }
    }
    struct stat fileStatBuffer;
    if(fstat(0, &fileStatBuffer) < 0)
    {
        fprintf(stderr, "Error using fstat!");
        exit(1);
    }


    // Check if input is file
    if(S_ISREG(fileStatBuffer.st_mode)){
        // Pass in file size for initial memory allocation
        int fileSize = (int) fileStatBuffer.st_size + 1;
        sfrob(fileSize, ignoreCase);
    }
    else{
        sfrob(0, ignoreCase);
    }

}
