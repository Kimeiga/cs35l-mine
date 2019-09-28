#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{

    srand(time(NULL));   // should only be called once


    for (int i = 0; i < 5000000; ++i) {
        int r = rand() % 57;
        r += 'A';
        putchar(r);
    }
}