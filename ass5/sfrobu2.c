#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int cmp_count = 0;

int frobcmp(const void* arr_a, const void* arr_b)
{
    // Keeps count of number of times words are compared/frobcmp() is called
    cmp_count++;

    // Casts pointer a and b
    const char * a = *(const char **) arr_a;
    const char * b = *(const char **) arr_b;

    while (1)
    {
        if ((*a == ' ') && (*b == ' ')) // Both at end of arrays, equal
        {
            return 0;
        }
        else if (*a == ' ') // End of array a, a < b
        {
            return -1;
        }
        else if (*b == ' ') // End of array b, a > b
        {
            return 1;
        }
        else
        {
            if ((*a ^ 42) < (*b ^ 42))  // Unfobricated a < b
            {
                return -1;
            }
            else if ((*a ^ 42) > (*b ^ 42)) // Unfobricated b < a
            {
                return 1;
            }
        }

        //else, continue checking
        a++;
        b++;
    }

    return 0;
}

void sfrob(int length)
{
    // Array of pointers to words
    char ** word_arr = (char**)malloc(0);
    int word_arr_i = 0;

    // Pointer to single word (delimited by space)
    char * word = (char*)malloc(length * sizeof(char));
    int file_i = 0;

    // Keep track of size of file
    int size = length;

    char cur_char = ' ';

    while (read(0, &cur_char, sizeof(char)) > 0)
    {
        // Allocate more memory if needed (file must have grown or is nonregular)
        if (file_i >= size)
        {
            size++;
            word = realloc(word, size * sizeof(char));
            if (word == NULL)
            {
                fprintf(stderr, "Error reallocating memory for chars!");
                exit(1);
            }
        }

        // Skip leading spaces in beginning
        if ((file_i == 0) && (cur_char == ' '))
        {
            continue;
        }
        // Skip consecutive spaces
        if ((file_i > 0) && (cur_char == ' ') && (word[file_i - 1] == ' '))
        {
            continue;
        }

        // Add char to word
        word[file_i] = cur_char;
        file_i++;
    }

    // If input ended without a ending space
    if (word[file_i - 1] != ' ')
    {
        // Add space to end
        size++;
        word = realloc(word, size * sizeof(char));
        if (word == NULL)
        {
            fprintf(stderr, "Error reallocating memory for chars!");
            exit(1);
        }
        word[file_i] = ' ';
        file_i++;
    }

    // Iterate through WORD and add pointers to WORD_ARRS
    int k = 0;
    int start_curr_word = 0;

    for (; k < file_i; k++)
    {
        // End of word
        if (word[k] == ' ')
        {
            // Reallocate space
            word_arr = realloc(word_arr, (word_arr_i + 1) * sizeof(char*));
            if (word_arr == NULL)
            {
                fprintf(stderr, "Error reallocating memory for pointers!");
                exit(1);
            }
            // Add complete word to the array
            word_arr[word_arr_i] = word + start_curr_word;
            word_arr_i++;

            // Set start address of next word
            start_curr_word = k + 1;
        }
    }

    // Sort frobnicated words
    qsort(word_arr, word_arr_i, sizeof(char*), frobcmp);

    int i = 0;
    int j = 0;
    char c;
    char space = ' ';

    // Output to stdout
    for (; i < word_arr_i; i++)
    {
        j = 0;
        for (; word_arr[i][j] != ' '; j++)
        {
            c = word_arr[i][j];
            //write(1, &c, 1);
            if(write(1, &c, 1) < 1)
            {
                fprintf(stderr, "Error while writing!");
                exit(1);
            }
        }
        //write(1, &space, 1);
        if(write(1, &space, 1) < 1)
        {
            fprintf(stderr, "Error while writing!");
            exit(1);
        }
    }

    // Free stuffs
    free(word);
    free(word_arr);

    // Print number of comparisons made
    fprintf(stderr, "Comparisons: %i\n", cmp_count);

    exit (0);
}

int main(void)
{
    struct stat fileStat;
    if(fstat(0,&fileStat) < 0)
    {
        fprintf(stderr, "Error using fstat!");
        exit(1);
    }

    // Check if input is a regular file
    if(S_ISREG(fileStat.st_mode)){
        // Pass in file size for initial memory allocation
        int fileLength = fileStat.st_size;
        sfrob(fileLength);
    }
    else{
        sfrob(0);
    }
}

