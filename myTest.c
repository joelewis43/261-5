#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>

void addWord(char ** array, int index, char* word) {
    array[index] = malloc(sizeof(word));
    strcpy(array[index], word);
}

void printWord(char ** array, int index) {
    printf("%s\n", array[index]);
}

void initWordArray(char ** wordArray) {
    addWord(wordArray, 0, "Hello");
    addWord(wordArray, 1, "its");
    addWord(wordArray, 2, "me");
    addWord(wordArray, 3, "i");
    addWord(wordArray, 4, "was");
    addWord(wordArray, 5, "wondering");
    addWord(wordArray, 6, "if");
    addWord(wordArray, 7, "after");
    addWord(wordArray, 8, "all");
    addWord(wordArray, 9, "these");
    addWord(wordArray, 10, "years");
    addWord(wordArray, 11, "youd");
    addWord(wordArray, 12, "like");
    addWord(wordArray, 13, "to");
    addWord(wordArray, 14, "meet");
    addWord(wordArray, 15, "too");
    addWord(wordArray, 16, "go");
    addWord(wordArray, 17, "over");
    addWord(wordArray, 18, "every");
    addWord(wordArray, 19, "thing");
}

void initValuesArray(int * values, int length) {
    int i;
    for (i=0; i<length; i++) {
        values[i] = i+1;
    }
}

void cleanUp(int* values, char ** wordArray, int length) {
    free(values);

    int i;
    for (i=0; i<length; i++) {
        free(wordArray[i]);
    }
}

int main() {

    int length = 20;
    char* wordArray[length];
    initWordArray(wordArray);

    int* values;
    values = malloc(length * sizeof(int));
    initValuesArray(values, length);

    HashMap* map = hashMapNew(10);

    int i;
    for (i=0; i<length; i++) {
        hashMapPut(map, wordArray[i], values[i]);
    }

    printf("\nInitial Print\n\n");
    hashMapPrint(map);

    resizeTable(map, 20);
    printf("1\n");
    hashMapPrint(map);
    resizeTable(map, 40);
    printf("2\n");
    resizeTable(map, 80);
    printf("3\n");
    resizeTable(map, 160);
    printf("4\n");
    resizeTable(map, 320);
    printf("5\n");

    printf("\nAfter Resize Print\n\n");
    hashMapPrint(map);

    hashMapDelete(map);

    cleanUp(values, wordArray, length);
    
    return 0;
}