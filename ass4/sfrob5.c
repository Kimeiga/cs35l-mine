#include <stdio.h>
#include <stdlib.h>

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

void sfrob() {
    // Pointer to single word (delimited by space)
    char * word = (char*)malloc(sizeof(char));
    int wordIterator = 0;
    // Array of pointers to wordArray
    char ** wordArray = (char**)malloc(sizeof(char*));
    int wordArrayIterator = 0;

    char currentChar = getchar();

    if(ferror(stdin))
    {
        fprintf(stderr, "Error while reading file!");
        exit(1);
    }

    char nextChar = getchar();

    if(ferror(stdin))
    {
        fprintf(stderr, "Error while reading file!");
        exit(1);
    }

    while(currentChar != EOF && !ferror(stdin)) { //read the input stream until EOF
        word[wordIterator] = currentChar; //add a letter to the word

        wordIterator++;


        //reallocate memory for word
        char *temp = realloc(word, (wordIterator + 1) * sizeof(char));
        if (temp != NULL) {
            //Make the word equal to the reallocated space
            word = temp;
        }
        else //Allocation error, print error and exit
        {
            free(word);
            fprintf(stderr, "Error Allocation Memory!");
            exit(1);
        }

        if (currentChar == ' ' && wordIterator > 1) //Hit the end of the word
        {
            wordArray[wordArrayIterator] = word; //Add word to wordArray list
            //Constantly reallocate space for growing wordArraylist
            char **anotherOne = realloc(wordArray, (wordArrayIterator + 2) * sizeof(char *));
            if (anotherOne != NULL) {
                //Make wordArray equal to reallocated space
                wordArray = anotherOne;
                wordArrayIterator++;
                //Set word back to empty by pointing it to other space
                word = NULL;
                word = (char *) malloc(sizeof(char));
                wordIterator = -1; //-1 to bring back to 0 when summed later
            }
            else //Allocation error, print error and exit
            {
                free(wordArray);
                fprintf(stderr, "Error Allocation Memory!");
                exit(1);
            }
        }
        if (nextChar == EOF && currentChar == ' ') {
            break;
        }
        else if (currentChar == ' ' && nextChar == ' ') //Ignore Extra Spaces
        {
            while (currentChar == ' ') {
                currentChar = getchar();
                if(ferror(stdin))
                {
                    fprintf(stderr, "Error while reading file!");
                    exit(1);
                };
            }
            nextChar = getchar();
            if(ferror(stdin))
            {
                fprintf(stderr, "Error while reading file!");
                exit(1);
            };
            wordIterator++;
            continue;
        }
        else if (nextChar == EOF) //Add a space at the end if there isn't already one
        {
            currentChar = ' ';
            wordIterator++;
            continue;
        }
        //increment our letter counter and get the next character
        currentChar = nextChar;
        nextChar = getchar();
        if(ferror(stdin))
        {
            fprintf(stderr, "Error while reading file!");
            exit(1);
        };
        wordIterator++;
    }



    // Sort frobnicated wordArray
    qsort(wordArray, wordArrayIterator, sizeof(char*), frobcmp);

    //Output the wordArray to stdout using putchar
    for(size_t i = 0; i < wordArrayIterator; i++)
    {
        for(size_t j = 0; wordArray[i][j] != ' '; j++)
        {
            putchar(wordArray[i][j]);
        }
        putchar(' ');
    }

    free(wordArray);
    exit(0);




}

int main(void)
{
    sfrob();
}
