#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
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
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    char *word = nextWord(file);
    int i = 1;

    while (word != NULL) {

        hashMapPut(map, word, i++);
        free(word);
        word = nextWord(file);

    }
}

int minimum(int a, int b, int c) {

    if (a < b) {
        if (a < c) {
            return a;
        }
        return c;
    }
    if (b < c) {
        return b;
    }
    return c;

}

// len_s and len_t are the number of characters in string s and t respectively
int LevenshteinDistance(const char *s, int lenS, const char *t, int lenT) { 
    int cost;

    /* base case: empty strings */
    if (lenS == 0) return lenT;
    if (lenT == 0) return lenS;

    /* test if last characters of the strings match */
    if (s[lenS-1] == t[lenT-1])
        cost = 0;
    else
        cost = 1;

    /* return minimum of delete char from s, delete char from t, and delete char from both */
    return minimum(LevenshteinDistance(s, lenS - 1, t, lenT    ) + 1,
                   LevenshteinDistance(s, lenS    , t, lenT - 1) + 1,
                   LevenshteinDistance(s, lenS - 1, t, lenT - 1) + cost);
}

/**
 * Traverses the map and calculates the Levenshtein distance for each word.
 * @param map
 * @param inputWord
 */
void traverseMap(HashMap* map, char* inputWord, char* suggestions[5]) {

    int i;
    int max = 1000;
    int x = 0;
    int distance;
    struct HashLink * current;

    //loop through the map
    for (i=0; i<map->capacity; i++) {

        //get first link of the row
        current = map->table[i];

        while (current != NULL) {

            //calculate the current words distance
            distance = LevenshteinDistance(current->key, strlen(current->key), inputWord, strlen(inputWord));

            //check if distance is small enough to be a suggestion
            if (distance <= max) {
                max = distance;
                suggestions[x] = current->key;
                x++;
                if (x == 5) { x = 0; }
            }

            //go to next link
            current = current->next;
        }
    }
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
    HashMap* map = hashMapNew(1000);
    
    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    printf("Size: %d\n", hashMapSize(map));
    printf("Cap: %d\n", hashMapCapacity(map));
    
    char inputBuffer[256];
    int i;
    int quit = 0;
    char* suggestions[5];
    while (!quit) {

        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        //check all characters are letters and convert to lowercase
        i = 0;
        while (inputBuffer[i] != '\0') {

            //uppercase letter, convert to lower
            if (inputBuffer[i] >= 'A' && inputBuffer[i] <= 'Z') {
                inputBuffer[i] += 32;
            }

            //lowercase letter
            else if (inputBuffer[i] >= 'a' && inputBuffer[i] <= 'z') {
                //do nothing
            }

            //invalid character
            else {
                printf("Invalid Input!\n");
                quit = 1;
                break;
            }

            //increment char counter
            i++;
        }

        //check if user is quiting
        if (strcmp(inputBuffer, "quit") == 0){
            quit = 1;
        }

        //check if value is in the dictionary
        if (hashMapContainsKey(map, inputBuffer)) {
            printf("The word \"%s\" is spelled correctly.\n", inputBuffer);
        }

        //word is misspelled and is a valid input
        else if (!quit){

            printf("The word \"%s\" is not spelled correctly.\n", inputBuffer);

            //traverse hash table, putting the levenshtein distances into the links
            //suggestions will contain the 5 closest matches
            traverseMap(map, inputBuffer, suggestions);

            //print suggestions
            printf("Did you mean:\n");
            for (i=0; i<5; i++) {
                printf("\"%s\"\n", suggestions[i]);
            }
        }
    }
    
    hashMapDelete(map);
    return 0;
}
