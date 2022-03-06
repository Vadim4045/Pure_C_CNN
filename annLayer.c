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

    newLayer->content = (double*) calloc(count, sizeof(double));
    if(newLayer->content == NULL){
        freeLayer(newLayer);
        return NULL;
    }

    if(position>0){

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
        layer->weights[i] = (double*) calloc(layer->count+1, sizeof(double));
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
        for(j=0;j<=layer->count;j++){
            layer->weights[i][j] = (((float)rand()/(float)RAND_MAX)-0.4)*0.1;
        }
    }
}

void layerFP(annLayer* layer){
    int i, j;

    #pragma omp parallel for
    for(i=0;i<layer->next->count;i++){
        layer->next->content[i]=layer->weights[i][0];
        layer->next->fallacy[i] = 0;

        for(j=1;j<=layer->count;j++){
            layer->next->content[i] += layer->content[j-1] * layer->weights[i][j];
        }

        layer->next->content[i] = sigma(layer->next->content[i], layer->alfa);    
    }
    #pragma omp barier
}

void layerFPOut(annLayer* layer){
    int i, j;
    double totalExp=0;

    #pragma omp parallel for
    for(i=0;i<layer->next->count;i++){
        layer->next->content[i]=layer->weights[i][0];
        layer->next->fallacy[i] = 0;

        for(j=1;j<=layer->count;j++){
            layer->next->content[i] += layer->content[j-1] * layer->weights[i][j];
        }

        layer->next->content[i] = exp(layer->next->content[i]);

        #pragma omp atomic
        totalExp += layer->next->content[i];
    }
    #pragma omp barier

    #pragma omp parallel for
    for(i=0;i<layer->next->count;i++){
        layer->next->content[i] /=totalExp;
    }
    #pragma omp barier
}

void layerBP(annLayer* layer, double mu, double wCount){
    unsigned int i,j;
   
   if(layer->fallacy != NULL){
        #pragma omp parallel for num_threads(4)
        for(i=0;i<layer->next->count;i++){
            for(j=1;j<=layer->count;j++){
                layer->fallacy[j-1] += layer->weights[i][j]*layer->next->fallacy[i];
            }
        }
        #pragma omp barier
    }
    

    #pragma omp parallel for
    for(i=0;i<layer->next->count;i++){
        layer->weights[i][0] = layer->weights[i][0]*(1-mu*mu/wCount) + mu*layer->next->fallacy[i]*sigmaPrime(layer->next->content[i], layer->alfa);

        for(j=1;j<=layer->count;j++){
            layer->weights[i][j] = layer->weights[i][j]*(1-mu*mu/wCount) + mu*layer->next->fallacy[i]*layer->content[j-1]*sigmaPrime(layer->next->content[i], layer->alfa);
        }
    }
    #pragma omp barier
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
    //return num>0? alfa*num:0;
    return  1.0/(1.0+exp(-alfa*num));
}

inline double sigmaPrime(double num, double alfa){
    //return num>0? alfa:0;
    return alfa*num*(1.0-alfa*num);
}