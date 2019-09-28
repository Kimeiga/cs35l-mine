#include<stdio.h>
#include<stdlib.h>

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

//Function to check for reading error
void readError()
{
    if(ferror(stdin))
    {
        fprintf(stderr, "Error while reading file!");
        exit(1);
    }
}

void sfrob() {
    char* word = (char*)malloc(sizeof(char)); //Holds one word at a time (delimited by spaces)
    char** words = (char**)malloc(sizeof(char*)); //Array to hold pointers to words

    char currentChar = getchar();
    readError();
    char nextChar = getchar();
    readError();
    int letterIterator = 0;
    int wordsIterator = 0;

    while(currentChar != EOF && !ferror(stdin)) //Read file until EOF
    {

        word[letterIterator] = currentChar; //Add letter

        char* temp = realloc(word, (letterIterator+2)*sizeof(char));
        if(temp != NULL)
        {
            //Make the word equal to the reallocated space
            word = temp;
        }
        else
        {
            free(word);
            fprintf(stderr, "Error Allocation Memory!");
            exit(1);
        }


        if(currentChar == ' ') //Hit the end
        {
            words[wordsIterator] = word; //Add word to words list

            char** wordsLarger = realloc(words, (wordsIterator+2)*sizeof(char*));
            if(wordsLarger != NULL)
            {
                
                words = wordsLarger;
                word = NULL;
                wordsIterator++;
                
                word = (char*)malloc(sizeof(char));
                letterIterator = -1; //-1 to bring back to 0 when summed later
            }
            else //Allocation error
            {
                free(words);
                fprintf(stderr, "Error Allocation Memory!");
                exit(1);
            }
        }
        if(nextChar == EOF)
        {
            if(currentChar != ' '){
                currentChar = ' ';
                letterIterator++;
                continue;
            }
            else{
                break;
            }

        }
        else if (currentChar == ' ' && nextChar == ' ')
        {
            while(currentChar == ' ')
            {
                currentChar = getchar();
                readError();
            }
            nextChar = getchar();
            readError();
            letterIterator++;
            continue;
        }
        
        currentChar = nextChar;
        nextChar = getchar();
        readError();
        letterIterator++;

    }


    // Sort frobnicated wordArray
    qsort(words, wordsIterator, sizeof(char*), frobcmp);

    //Output the wordArray to stdout using putchar
    for(size_t i = 0; i < wordsIterator; i++)
    {
        for(size_t j = 0; words[i][j] != ' '; j++)
        {
            putchar(words[i][j]);
        }
        putchar(' ');
    }

    free(words);
    exit(0);
}


int main(void)
{
    sfrob();
}
