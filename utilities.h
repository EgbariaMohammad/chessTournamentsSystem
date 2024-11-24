#ifndef utilities_h
#define utilities_h

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

typedef void* VoidPtr;

VoidPtr copyIntKey(VoidPtr key);
void freeIntKey(VoidPtr id);
int compareIntKey(VoidPtr id1, VoidPtr id2);

VoidPtr copyDoubleData(VoidPtr key);
void freeDoubleData(VoidPtr id);
int Doublekeycompare(VoidPtr id1, VoidPtr id2);


#endif /* utilities_h */
