#include "annLayer.h"

double sigma(double, double);
double sigmaPrime(double, double);
void* fpThread(void*);
void* bpThread(void*);

annLayer* newLayer(int count, double alfa, int position){
    unsigned int i;

    annLayer* newLayer = (annLayer*) calloc(1, sizeof(annLayer));
    if (newLayer==NULL) {
        return NULL;
    }

    newLayer->count = count;
    newLayer->alfa=alfa;

    if(position>0){
        newLayer->content = (double*) calloc(count, sizeof(double));
        if(newLayer->content == NULL){
            freeLayer(newLayer);
            return NULL;
        }

        newLayer->fallacy = (double*) calloc(count, sizeof(double));
        if(newLayer->fallacy == NULL){
            freeLayer(newLayer);
            return NULL;
        }
    }

    return newLayer;
}

annLayer* layerMakeContinue(annLayer* layer, annLayer* nextLayer){

    int i;

    layer->next = nextLayer;

    layer->weights = (double**) calloc(nextLayer->count, sizeof(double*));
    if(layer->weights == NULL){
        freeLayer(layer);
        return NULL;
    }

    for(i=0;i<nextLayer->count;i++){
        layer->weights[i] = (double*) calloc(layer->count, sizeof(double));
        if(layer->weights[i]==NULL){
            freeLayer(layer);
            return NULL;
        }
    }

    return layer;
}

void randomWeights(annLayer* layer){
    unsigned int i, j;

    srand(time(NULL));

    for(i=0;i<layer->next->count;i++){
        for(j=0;j<layer->count;j++){
            layer->weights[i][j] = (((float)rand()/(float)RAND_MAX)-0.5)*0.1;
        }
    }
}

void layerFP(annLayer* layer){
    int i, j;

    for(i=0;i<layer->next->count;i++){
        layer->next->content[i]=0;
        layer->next->fallacy[i] = 0;

        for(j=0;j<layer->count;j++){
            layer->next->content[i] += layer->content[j] * layer->weights[i][j];
        }

        layer->next->content[i] = sigma(layer->next->content[i], layer->alfa);       
    }
}

void layerBP(annLayer* layer, double mu){
    unsigned int i,j;
   
    for(i=0;i<layer->next->count;i++){
        for(j=0;j<layer->count;j++){
            if(layer->fallacy != NULL){
                layer->fallacy[j] += layer->weights[i][j]*layer->next->fallacy[i];
            }

            layer->weights[i][j] += mu*layer->next->fallacy[i]*sigmaPrime(layer->next->content[i], layer->alfa)*layer->content[j];
        }
    }

}

int freeLayer(annLayer* layer){
    unsigned int i;

    if(layer == NULL){
        return 0;
    }

    if(layer->fallacy != NULL){
        free(layer->fallacy);
        free(layer->content);
    }

    if(layer->weights != NULL){
        for(i=0;i<layer->next->count;i++){
            if(layer->weights[i] != NULL){
                free(layer->weights[i]);
            }
            
        }

        free(layer->weights);
    }

    free(layer);

    return 1;
}

inline double sigma(double num, double alfa){
    return num>0? alfa*num:0.01*num;//1.0/(1.0+exp(-alfa*num));
}

inline double sigmaPrime(double num, double alfa){
    return num>0? alfa:0.01*alfa;//alfa*num*(1.0-alfa*num);
}