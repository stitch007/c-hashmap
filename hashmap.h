#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct hashMap *HashMap;

typedef int (*HashCode)(HashMap, char *key);

typedef void (*Set)(HashMap hashMap, char *key, void *value);

typedef void *(*Get)(HashMap hashMap, char *key);

typedef bool (*Has)(HashMap hashMap, char *key);

typedef bool (*Delete)(HashMap hashMap, char *key);

typedef void (*Clear)(HashMap hashMap);

typedef struct hashMapNode {
    char *key;
    void *value;
    struct hashMapNode *next;
} *HashMapNode;

typedef struct hashMap {
    unsigned size;
    unsigned capacity;
    HashCode hashCode;
    HashMapNode nodes;
    Set set;
    Get get;
    Has has;
    Delete delete;
    Clear clear;
} *HashMap;

typedef struct hashMapIterator *HashMapIterator;

typedef HashMapIterator (*Next)(HashMapIterator iterator);

typedef bool (*HasNext)(HashMapIterator iterator);

typedef struct hashMapIterator {
    HashMap hashMap;
    HashMapNode node;
    unsigned count;
    int hashCode;
    Next next;
    HasNext hasNext;
} *HashMapIterator;

HashMap createHashMap();

void deleteHashMap(HashMap *hashMap);

HashMapIterator createHashMapIterator(HashMap hashMap);

void deleteHashMapIterator(HashMapIterator *iterator);
