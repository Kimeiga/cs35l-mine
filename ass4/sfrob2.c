#include <stdio.h>
#include <stdlib.h>

int frobcmp(const void* arr_a, const void* arr_b)
{
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

void sfrob()
{
    // Pointer to single word (delimited by space)
    char * word = (char*)malloc(0);
    int word_i = 0;
    // Array of pointers to words
    char ** word_arr = (char**)malloc(0);
    int word_arr_i = 0;

    char cur_char  = ' ';

    while ((!feof(stdin)) && (!ferror(stdin)))
    {
        // Add to word
        cur_char = getchar();
        if (ferror(stdin))
        {
            fprintf(stderr, "Error reading in file!");
            exit(1);
        }
        if (cur_char == EOF)
        {
            break;
        }
        if ((cur_char == ' ') && (word_i == 0))
        {
            continue;
        }

        word = realloc(word, (word_i + 1) * sizeof(char));
        //char c = cur_char;
        //printf("Adding %c to word %d", c, word_arr_i);
        word[word_i] = cur_char;
        word_i++;
        if (word == NULL)
        {
            fprintf(stderr, "Error reallocating memory for chars!");
            exit(1);
        }

        // End of word
        if ((cur_char == ' ') && (word_i > 1))
        {
            // Reallocate space
            word_arr = realloc(word_arr, (word_arr_i + 1) * sizeof(char*));
            // Add complete word to the array
            word_arr[word_arr_i] = word;
            word_arr_i++;
            if (word_arr == NULL)
            {
                fprintf(stderr, "Error reallocating memory for pointers!");
                exit(1);
            }

            // Empty word
            word = (char*)malloc(0);
            word_i = 0;
        }
    }

    // Check if last word added (might not have ended with ' ')
    if (word_i != 0)
    {
        // Add space to end
        word = realloc(word, (word_i + 1) * sizeof(char));
        word[word_i] = ' ';
        word_i++;
        if (word == NULL)
        {
            fprintf(stderr, "Error reallocating memory for chars!");
            exit(1);
        }

        // Reallocate space
        word_arr = realloc(word_arr, (word_arr_i + 1) * sizeof(char*));
        // Add complete word to the array
        word_arr[word_arr_i] = word;
        word_arr_i++;
        if (word_arr == NULL)
        {
            fprintf(stderr, "Error reallocating memory for pointers!");
            exit(1);
        }

        // Empty word
        word = (char*)malloc(0);
        word_i = 0;
    }

    // Sort frobnicated words
    qsort(word_arr, word_arr_i, sizeof(char*), frobcmp);

    int i = 0;
    int j = 0;
    // Output to stdout
    for (; i < word_arr_i; i++)
    {
        j = 0;
        for (; word_arr[i][j] != ' '; j++)
        {
            putchar(word_arr[i][j]);
        }
        putchar(' ');
    }

    // Deallocate all space
    free(word_arr);

    exit (0);
}

int main(void)
{
    sfrob();
}

