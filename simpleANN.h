#ifndef H_ANN
#define H_ANN
#define NELEMS(x)  (int)(sizeof(x) / sizeof((x)[0]))

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "annLayer.h"

typedef struct simpleANN
{
    int layersCount;
    int* configArr;
    annLayer** innerLayers;
    const char* weights_folder;
    double* totalFallacy;
    double weightCount;
    int epoch;
} Ann;

Ann* newSimpleANN(int, int*, double, const char*);
int simpleAnnGo(Ann*);
int simpleAnnLearn(Ann*, int, double);
void exportStoredWeights(Ann*, int, int, int);
int checkNumber(Ann*, double*);
void zeroTotalFallacy(Ann*);
void showTotalFallacy(Ann*,int);
int freeSimpleANN(Ann*);

#endif //H_ANN