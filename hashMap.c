/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: 
 * Date: 
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    int i;
    for (i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    int i;
    for (i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    int i;
    for (i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    int i;
    HashLink * current;
    HashLink * next;

    //loop through the table
    for (i=0; i<map->capacity; i++) {

        current = map->table[i];

        while (current != NULL) {
            next = current->next;

            //free current link
            hashLinkDelete(current);

            current = next;
        }
        
        map->table[i] = NULL;
    }

    //set size to zero
    map->size = 0;

    //free table
    free(map->table);
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    //get hash container index
    int index = HASH_FUNCTION(key) % map->capacity;

    //check if valid index
    if(index < 0) { index += map->capacity; }

    //store first hash link
    HashLink * current = map->table[index];

    //loop through the designated bucket
    while (current != NULL) {

        //check current key
        if (strcmp(current->key, key) == 0)
            return &(current->value);

        //advance current
        current = current->next;
    }

    //no match found
    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given 
 * capacity (double of the old capacity). After allocating the new table, 
 * all of the links need to rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{

    //create new table with new capacity
    HashMap* newMap = hashMapNew(capacity);

    int i;
    HashLink* current;

    //loop over old table
    for (i=0; i<map->capacity; i++) {

        //set to first hash link
        current = map->table[i];

        //loop through bucket
        while (current != NULL) {

            //add current value to new map
            hashMapPut(newMap, current->key, current->value);

            //advance pointer
            current = current->next;
        }
    }

    /*
        Im putting this extended explination here because I was stuck on this bug for 2 days.
        I originally was deleting map then dereferencing and assigning the contents for
        newMaps's stored address. I now realize this is wrong because when map is deleted,
        so to is the memory allocated to store a map struct. Therefore, I only clean up
        the inside of map (freeing all table space) then copy the contents of newMap
        into the map pointer. Finally, I free the space allocated for newMap as its contents
        are now referenced by the map pointer.
    */

    //free space inside of map (all links and the table)
    hashMapCleanUp(map); 

    //copy contents of newMap to original map
    *map = *newMap;

    //free the newMap pointer (only frees the map alloc and not the links or table)
    free(newMap);
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    //check for resize
    if(hashMapTableLoad(map) >= MAX_TABLE_LOAD) {
        resizeTable(map, 2*map->capacity);
    }

    //get hash container index
    int index = HASH_FUNCTION(key) % map->capacity;

    //check if valid index
    if(index < 0) { index += map->capacity; }

    //store first hash link of the correct bucket
    HashLink * current = map->table[index];

    int foundIt = 0;

    //loop through the designated bucket
    while (current != NULL) {

        //check if key match
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            foundIt = 1;
        }
        
        //advance current
        current = current->next;
    }

    if(!foundIt) {
        //insert new link before first element
        HashLink* newLink = hashLinkNew(key, value, map->table[index]);

        //point table to new link
        map->table[index] = newLink;

        //increment size
        map->size++;
    }
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    //get hash container index
    int index = HASH_FUNCTION(key) % map->capacity;

    //check if valid index
    if(index < 0) { index += map->capacity; }

    //store first hash link of the correct bucket
    HashLink * current = map->table[index];

    //if the first value is a match
    if (strcmp(current->key, key) == 0) {
        map->table[index] = current->next;
        hashLinkDelete(current);
        map->size--;
    }
    else {

        //store the previous link
        HashLink* prev = current;
        current = current->next;

        //loop through the designated bucket
        while (current != NULL) {

            //check if key match
            if (strcmp(current->key, key) == 0) {
                prev->next = current->next;
                hashLinkDelete(current);
                map->size--;
                break;
            }

            //advance pointers
            prev = current;
            current = current->next;
        }
    }
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    //get hash container index
    int index = HASH_FUNCTION(key) % map->capacity;

    //check if valid index
    if(index < 0) { index += map->capacity; }

    //store first hash link of the correct bucket
    HashLink * current = map->table[index];

    //loop through the designated bucket
    while (current != NULL) {

        //check if key match
        if (strcmp(current->key, key) == 0) {
            return 1;
        }

        //advance current
        current = current->next;
    }

    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    int count = 0;

    int i;
    for (i=0; i<map->capacity; i++)
        if (map->table[i] == NULL)
            count++;
    
    return count;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    float n, cap, result;

    n = (float) map->size;
    cap = (float) map->capacity;

    result = n / cap;


    return result;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
    HashLink* current;
    int i;
    for (i=0; i<map->capacity; i++) {

        current = map->table[i];

        printf("Bucket %d -> ", i);

        while (current != NULL) {

            printf("(%s, %d) -> ", current->key, current->value);
            current = current->next;

        }
        printf("\n");
    }
}
