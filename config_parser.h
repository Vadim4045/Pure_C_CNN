#ifndef CONF_PARS_H
#define CONF_PARS_H

#define BUFF_BIG 100
#define BUFF_SMALL 50

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct _config
{
    int in_rows;
    int in_columns;
    int masks_count;
    int mask_rows;
    int mask_columns;
    int conv_step;
    int reduse;
    char masks_file[BUFF_SMALL];
    int layers;
    int layers_counts[5];
    double alfa;
    double mu;
    char data[BUFF_SMALL];
    int data_len;
    char weights[BUFF_SMALL];
    int epochs;
    int mode;
}config;

config* parseCommand(int, char**);
void printDataSet(int, int, double*);
//void printConfig(config*);


#endif //CONF_PARS_H