#ifndef _MAIN
#define _MAIN

#define BUFF_BIG 100
#define BUFF_SMALL 50


#include <stdio.h>
#include <stdlib.h>
#include <mcheck.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include "config_parser.h"
#include "dataImport.h"
#include "convolutionLayer.h"
#include "simpleANN.h"

double** shuffleData(double**, double**, int);

int main(int argc, char *argv[]){
    Ann* ann;
    convolution_layer* conv_layer, *conv_layer2;
    double** data, **newData, **tmp, *between;
    time_t start_t, end_t;
    double diff_t;
    config* conf;
    int i, j, res, num1, num2, good, old=0;

    mtrace();
    srand(time(NULL));

    if(argc==2){
        conf = parseCommand(argc, argv);
    }else{
        printf("Not enough parameters.\n");
        exit(0);
    }
    if (conf == NULL){
        printf("Config not readed.\n");
        exit(0);
    }

    data = getData(conf->data, &conf->data_len);
    if(data == NULL){
        perror("Dataset reading error");
        exit(0);
    }

    newData = (double**)calloc(conf->data_len, sizeof(double*));
    if(newData == NULL){
        perror("Dataset reading error");
        freeDataMemory(data, conf->data_len);
        exit(0);
    }

    conv_layer = newConvolutionLayer(conf);
    if(conv_layer == NULL){
        perror("conv_layer");
        freeDataMemory(data, conf->data_len);
        exit(0);
    }

    /* between = (double*)calloc(conf->layers_counts[0], sizeof(double));
    if(between == NULL){
        perror("conv_layer");
        freeConvolutionLayer(conv_layer);
        freeDataMemory(data, conf->data_len);
        exit(0);
    }

    conv_layer2 = newConvolutionLayer(conf);
    if(conv_layer2 == NULL){
        perror("conv_layer");
        freeConvolutionLayer(conv_layer);
        free(between);
        freeDataMemory(data, conf->data_len);
        exit(0);
    }

    setInLayer(conv_layer2,between);
    setOutLayer(conv_layer,between); */

    ann = newSimpleANN(conf->layers, conf->layers_counts, conf->alfa, conf->weights);

    setOutLayer(conv_layer, ann->innerLayers[0]->content);

    if(conf->mode==0){
        for(i = 1; i <= conf->epochs; i++){
            time(&start_t);

            tmp = shuffleData(data, newData, conf->data_len);
            newData=data;
            data=tmp;

            good=0;

            zeroTotalFallacy(ann);

            for(j=0;j<conf->data_len;j++){
                res = checkNumber(ann, data[j] + 784);
                setInLayer(conv_layer, data[j]);

                doConvolution(conv_layer);

                if(simpleAnnLearn(ann, res, conf->mu)) good++;
            }
            
            exportStoredWeights(ann, i, conf->data_len, good);

            //conf->mu*=0.99;
            old=good;

            time(&end_t);
            diff_t = difftime(end_t, start_t);

            showTotalFallacy(ann,conf->data_len);

            printf("Step - %d from %d: %d/%d on %f sec.\n", i, conf->epochs, conf->data_len, good, diff_t);
        }
    }else if(conf->mode==1){
        time(&start_t);

            good=0;

            for(j=0;j<conf->data_len;j++){
                res = checkNumber(ann, data[j] + 784);
                setInLayer(conv_layer, data[j]);

                doConvolution(conv_layer);

                if(simpleAnnLearn(ann, res, conf->mu)) good++;
            }

            time(&end_t);
            diff_t = difftime(end_t, start_t);

            printf("Step - %d from %d: %d/%d on %f sec.\n", i, conf->epochs, conf->data_len, good, diff_t);
        
    }
    
    freeConvolutionLayer(conv_layer);
    freeSimpleANN(ann);
    freeDataMemory(data, conf->data_len);

    exit(0);
}

double** shuffleData(double** data, double** newData, int count){
    unsigned int i, j, rnd;

    for(i=0;i<count;i++){
        rnd=(rand()*count)/RAND_MAX;

        while (newData[rnd]!=NULL) {
            if(rnd==count-1) rnd=0;
            else rnd++;
        }

        newData[rnd] = data[i];
        data[i] = NULL;

    }

    return newData;
}
#endif // _MAIN