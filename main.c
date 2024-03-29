#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL if EOF.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1) {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')  
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            if ((c >= 'A' && c <= 'Z'))
            {
                c += 32; //convert to lowercase
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Prints the concordance of the given file and performance information. Uses
 * the file input1.txt by default or a file name specified as a command line
 * argument.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    const char* fileName = "input2.txt";
    if (argc > 1)
    {
        fileName = argv[1];
    }
    printf("Opening file: %s\n", fileName);
    FILE* file;
    file = fopen(fileName, "r");
    
    clock_t timer = clock();
    
    HashMap* map = hashMapNew(10);
    


    // --- Concordance code begins here ---

    char* word = nextWord(file);

    while(word != NULL) {

        //printf("%s\n", word);
        
        if (hashMapContainsKey(map, word)) {
            //puts the current value plus one
            hashMapPut(map, word, (*hashMapGet(map, word))+1);
        }
        else {
            hashMapPut(map, word, 1);
        }

        //free word
        free(word);

        //get next word
        word = nextWord(file);

    }

    hashMapPrint(map);

    // --- Concordance code ends here ---
    


    timer = clock() - timer;
    printf("\nRan in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    printf("Empty buckets: %d\n", hashMapEmptyBuckets(map));
    printf("Number of links: %d\n", hashMapSize(map));
    printf("Number of buckets: %d\n", hashMapCapacity(map));
    printf("Table load: %f\n", hashMapTableLoad(map));

    fclose(file);
    
    hashMapDelete(map);
    return 0;
}