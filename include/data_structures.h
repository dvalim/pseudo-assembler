#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//DEFINICJA DYNAMICZNEJ TABLICY

#define VECTOR(TYPE) struct { \
    int size, capacity; \
    TYPE* data; \
}

#define V_INIT(V, TYPE) do { \
    V.size = 0; \
    V.capacity = 1; \
    V.data = malloc(sizeof(TYPE) * V.capacity); \
} while(0) 

#define V_RESIZE(V, TYPE, NEW_CAPACITY) do { \
    V.data = realloc(V.data, sizeof(TYPE) * NEW_CAPACITY); \
    V.capacity = NEW_CAPACITY; \
} while(0)

#define V_ADD(V, TYPE, ITEM) do { \
    if(V.size == V.capacity) V_RESIZE(V, TYPE, V.capacity * 2); \
    V.data[V.size++] = ITEM; \
} while(0)

#define V_ADD_STRING(V, TYPE, ITEM) do { \
	if(V.size == V.capacity) V_RESIZE(V, TYPE, V.capacity * 2); \
	strcpy(V.data[V.size++], ITEM); \
} while(0)

#define V_FREE(V) do { \
    free(V.data); \
} while(0)

#define MAX_LENGTH 100

typedef struct {
	int size, capacity;
	char (*data)[MAX_LENGTH];
} string_vector;

typedef VECTOR(int) array;
typedef VECTOR(char) char_vector;

char_vector raw_code;
string_vector code_lines;

//FUNKCJE POMOCNICZE

int stringEquals(char[], char []);
int isCommand(char[]);
int sgn(int v);
int _max(int, int); //vstudio definiuje własny max
int numLength(int);

//MAPA

typedef struct {
	char key[MAX_LENGTH];
	int value;
} node;

typedef VECTOR(node) map;

node makeNode(char[], int);
int compareNodes(const void*, const void*);
int find(map, char[]);

#endif