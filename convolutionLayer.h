#ifndef H_CONV
#define H_CONV

#define BUFF 1000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_parser.h"

typedef struct _convolution_layer
{
    config* conf;
    int conv_rows;
    int conv_columns;
    double* masks;
    double* in_arr;
    double* out_arr;

}convolution_layer;

convolution_layer* newConvolutionLayer(config* conf);
void setInLayer(convolution_layer*,double*);
void setOutLayer(convolution_layer*,double*);
void doConvolution(convolution_layer*);
void freeConvolutionLayer(convolution_layer*);

#endif //H_CONV