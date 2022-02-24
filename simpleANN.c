#include "simpleANN.h"


int importStoredWeights(Ann*, const char*);
char* getLastWeightsFileName(Ann*, const char*);
int randomGenerateWeights(Ann* ann);
int annResult(Ann*);
void annBP(Ann*, double);

Ann* newSimpleANN(int count, int* config, double alfa, const char* weightsFolder){
    unsigned int i;

    Ann* ann = (Ann*) calloc(1, sizeof(Ann));
    if(ann==NULL){
        return NULL;
    }
    printf("New ANN: %d %d %d %d %d\n",count, config[0], config[1], config[2], config[3]);
    ann->configArr = config;
    ann->layersCount = count;
    ann->weights_folder = weightsFolder;

    ann->innerLayers = (annLayer**) calloc(count, sizeof(annLayer*));
    if(ann->innerLayers==NULL){
        freeSimpleANN(ann);
        return NULL;
    }

    for(i=0;i<count;i++){
        ann->innerLayers[i] = newLayer(config[i], alfa, i);

        if(ann->innerLayers[i] != NULL && i>0){

            ann->innerLayers[i-1] = layerMakeContinue(ann->innerLayers[i-1], ann->innerLayers[i]);

            if(ann->innerLayers[i-1] == NULL){
                freeSimpleANN(ann);
                return NULL;
            }
        }
    }

    if(importStoredWeights(ann, weightsFolder) == 0 && randomGenerateWeights(ann) == 0){
        freeSimpleANN(ann);
        return NULL;
    }

    return ann;
}

int simpleAnnGo(Ann* ann){
    int i, j;
    double* nextLayer; 

    for(i=0;i<ann->layersCount-1;i++){
        layerFP(ann->innerLayers[i]);
    }

return annResult(ann);
}

int annResult(Ann* ann){
    unsigned int i, idx;
    double max=-100000;

    for(i=0;i<ann->configArr[ann->layersCount-1];i++){
        if(ann->innerLayers[ann->layersCount-1]->content[i]>max){
            max = ann->innerLayers[ann->layersCount-1]->content[i];
            idx=i;
        }
    }
    return idx;
}

int simpleAnnLearn(Ann* ann, int res, double mu){
    int i, j, good=0;

    if (simpleAnnGo(ann) == res){
        good++;
    }

    for(j=0;j<ann->configArr[ann->layersCount-1];j++){
        if(j == res){
            ann->innerLayers[ann->layersCount-1]->fallacy[j] = 1-ann->innerLayers[ann->layersCount-1]->content[j];
        }else{
            ann->innerLayers[ann->layersCount-1]->fallacy[j] = -ann->innerLayers[ann->layersCount-1]->content[j];
        }
    }

    annBP(ann, mu);   

    return good;
}

void annBP(Ann* ann, double mu){
    int i;
    
    for(i=ann->layersCount-2;i>=0;i--){
        layerBP(ann->innerLayers[i], mu);
    }
}

int checkNumber(Ann* ann, double* data){
    unsigned int i;

    for(i=0;i<ann->configArr[ann->layersCount-1];i++){
        if(data[i]==1){
            return i;
        }
    }

    return -1;
}

int freeSimpleANN(Ann* ann){
    unsigned int i;

    if(ann==NULL){
        return 0;
    }

    for(i=0;i<ann->layersCount;i++){
        if(ann->innerLayers[i] != NULL){
            freeLayer(ann->innerLayers[i]);
        }
    }

    free(ann->innerLayers);
    free(ann);

    return 1;
}

int importStoredWeights(Ann* ann, const char* directory){
    unsigned int i, j, k;
    FILE *file;
    char* weightsFileName;
    size_t s;

    if(directory==NULL){
        directory="./";
    }

    weightsFileName = getLastWeightsFileName(ann, directory);

    file = fopen(weightsFileName,"rb");
    if(file==NULL){
        return 0;
    }

    for(i=0;i<ann->layersCount-1;i++){
        for(j=0;j<ann->innerLayers[i]->next->count;j++){
            for(k=0;k<ann->innerLayers[i]->count;k++){
                s=fread(&ann->innerLayers[i]->weights[j][k], sizeof(double), 1, file);
            }
        }
    }

    fclose(file);
    
    printf("Loaded stored weights from %s\n", weightsFileName);

    free(weightsFileName);

    return 1;
}

char* getLastWeightsFileName(Ann* ann, const char* directory){
    int i, epoch=0;
    DIR *d;
    struct dirent *dir;
    char confStr[32], tmp[7], tmp2[50], maxFile[50];
    char* weightsFileName;

    weightsFileName = (char*) calloc(50, sizeof(char));
    if(weightsFileName==NULL) return NULL;

    strcpy(weightsFileName, directory);

    strcpy(confStr, "_");
    for(i=0;i<ann->layersCount;i++){
        snprintf(tmp, sizeof(tmp), "%d_", ann->configArr[i]);
        strcat(confStr, tmp);
    }

    d = opendir(directory);
    if (d) {
        maxFile[0] = '\0';

        while ((dir = readdir(d)) != NULL) {

            if(strstr(dir->d_name, confStr) != NULL && strstr(dir->d_name, ".bin") != NULL){
                strcpy(tmp2,dir->d_name);
                strcpy(tmp,strtok(tmp2, "_"));

                if(atoi(tmp)>epoch){
                    epoch = atoi(tmp);
                    strcpy(maxFile, dir->d_name);
                }
            }
        }
        closedir(d);
    }

    if(maxFile[0] != '\0') {
        strcat(weightsFileName, maxFile);

        ann->epoch = epoch;

        return weightsFileName;
    }

    free(weightsFileName);
    return NULL;
}

int randomGenerateWeights(Ann* ann){
    unsigned int i;

    for(i=0;i<ann->layersCount-1;i++){
        randomWeights(ann->innerLayers[i]);   
    }

    printf("Random generated weights.\n");
    return 1;
}

void exportStoredWeights(Ann* ann, int epoch, int dataSetLength, int good){
    unsigned int i, j, k;
    FILE *file;
    char buffer[100], confStr[32], tmp[7];
    struct stat st = {0};

    strcpy(confStr, "_");
    for(i=0;i<ann->layersCount;i++){
        snprintf(tmp, sizeof(tmp), "%d_", ann->configArr[i]);
        strcat(confStr, tmp);
    }

    if (stat(ann->weights_folder, &st) == -1) {
        mkdir(ann->weights_folder, 0700);
    }

    snprintf(buffer, sizeof(buffer), "%s%.4d%s%.5d_%.5d.bin", ann->weights_folder, ann->epoch+epoch, confStr, dataSetLength, good);

    file = fopen(buffer,"wb+");
    if(file==NULL){
        return;
    }

    for(i=0;i<ann->layersCount-1;i++){
        for(j=0;j<ann->innerLayers[i]->next->count;j++){
            for(k=0;k<ann->innerLayers[i]->count;k++){
                fwrite(&ann->innerLayers[i]->weights[j][k], sizeof(double), 1, file);
            }
        }
    }

    fclose(file);

    //return;

    snprintf(buffer, sizeof(buffer), "%s%.4d%s%.5d_%.5d.txt", ann->weights_folder, ann->epoch+epoch, confStr, dataSetLength, good);

    file = fopen(buffer,"w+");
    if(file==NULL){
        return;
    }

    for(i=0;i<ann->layersCount-1;i++){
        for(j=0;j<ann->innerLayers[i]->next->count;j++){
            for(k=0;k<ann->innerLayers[i]->count;k++){
                fprintf(file, "%f, ", ann->innerLayers[i]->weights[j][k]);
            }
        }
    }

    fclose(file);

}