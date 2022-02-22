#ifndef H_LAYER
#define H_LAYER

#define MIN_PER_THREAD 20

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct _annLayer
{
    unsigned int count;
    double alfa;
    struct _annLayer* next;
    double* content;
    double* fallacy;
    double** weights;
} annLayer;

annLayer* newLayer(int, double, int);
annLayer* layerMakeContinue(annLayer*, annLayer*);
void layerFP(annLayer*);
void layerBP(annLayer*, double);
int freeLayer(annLayer*);
void randomWeights(annLayer*);

#endif //H_LAYER