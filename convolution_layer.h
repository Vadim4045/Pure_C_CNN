#ifndef H_CONV
#define H_CONV

#define BUFF_BIG 200
#define BUFF_SMALL 100

#include <stdio.h>

typedef struct _convolution_layer
{
    int in_rows;
    int in_columns;
    int step;
    int reduse;// 0-not, 1-yes
    int outSize;
    int mask_count;
    conv_mask* mask;
    double* in_arr;
    double* intermediate;
    double* out_arr;

}convolution_layer;

convolution_layer* newConvolutionLayer(int, int, int, int, int, int);
void doConvolution(convolution_layer*);
void freeConvolutionLayer(convolution_layer*);

#endif //H_CONV