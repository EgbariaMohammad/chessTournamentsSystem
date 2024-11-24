#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "utilities.h"

VoidPtr copyIntKey(VoidPtr key) {
   if (!key) return NULL;
    int* newKey = malloc(sizeof(*newKey));
    if (!newKey) return NULL;
    *newKey = *(int*)key;
    return newKey;
}

void freeIntKey(VoidPtr id) {
    free( (int*) id) ;
}

int compareIntKey(VoidPtr id1, VoidPtr id2) {
    return (*(int*) id1 - *(int*)id2);
}

VoidPtr copyDoubleData(VoidPtr key) {
    if (!key) return NULL;
    int* newKey = malloc(sizeof(*newKey));
    if (newKey  == NULL) {
        return NULL;
    }
    *newKey  = *((double*) key);
    return newKey;
}

void freeDoubleData(VoidPtr id) {
    free( (double*) id);
}

int Doublekeycompare(VoidPtr id1, VoidPtr id2) {
   return (*(double*)id1 - *(double*)id2);
}

