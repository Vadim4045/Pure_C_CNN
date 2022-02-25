#include "convolutionLayer.h"

void normalize(double*, int);
double* getConvolutionMasks(const char*,int,int,int);

convolution_layer* newConvolutionLayer(config* conf)
{
    int conv_rows, conv_columns, red_rows, red_columns;
    convolution_layer* layer = (convolution_layer*)calloc(1,sizeof(convolution_layer));
    if(layer ==NULL) return NULL;

    layer->conf = conf;

    layer->masks = getConvolutionMasks(conf->masks_file, conf->mask_rows, conf->mask_columns, conf->masks_count);
    if(layer->masks ==NULL){
        free(layer);
        return NULL;
    }
   
    layer->conv_rows = conf->in_rows/conf->conv_step+conf->mask_rows/2/conf->conv_step;
    layer->conv_columns = conf->in_columns/conf->conv_step+conf->mask_columns/2/conf->conv_step;

    layer->conf->layers_counts[0] = conf->masks_count*layer->conv_rows*layer->conv_columns;

    return layer;
}

void doConvolution(convolution_layer* layer)
{
    int cur_mask, i,j,k,l;
    double *out, *mask;
    for(cur_mask=0;cur_mask<layer->conf->masks_count;cur_mask++){

        out = &layer->out_arr[cur_mask*layer->conf->mask_rows*layer->conf->mask_columns];
        mask = &layer->masks[cur_mask*layer->conf->mask_rows*layer->conf->mask_columns];

        for(i=-layer->conf->mask_rows/2;i<layer->conf->in_rows-layer->conf->mask_rows/2;i+=layer->conf->conv_step){
            for(j=-layer->conf->mask_columns/2;j<layer->conf->in_columns-layer->conf->mask_columns/2;j+=layer->conf->conv_step){

                out[(i+layer->conf->mask_rows/2)* layer->conf->mask_columns/layer->conf->conv_step+(j+layer->conf->mask_columns/2)/layer->conf->conv_step]=0;

                for(k=0;k<layer->conf->mask_rows;k++){
                    for(l=0;l<layer->conf->mask_columns;l++){

                        if(i+k>=0 && j+l>=0 && i+k<layer->conf->in_rows && j+l<layer->conf->in_columns){

                            out[(i+layer->conf->mask_rows/2)* layer->conf->mask_columns/layer->conf->conv_step\
                            +(j+layer->conf->mask_columns/2)/layer->conf->conv_step] \
                            += layer->in_arr[(i+k)*layer->conf->in_columns+j+l]*mask[k*layer->conf->mask_columns+l];
                        }
                    }
                }
            }
        }

        normalize(out, layer->conv_rows*layer->conv_columns);
    }
}

inline void normalize(double* mat, int count){
    int i;
    double min, max;

    max=mat[0];

    for(i=0;i<count;i++){
        if(mat[i]<min) min=mat[i];
        if(mat[i]>max) max=mat[i];
    }

    for(i=0;i<count;i++){
        mat[i] = mat[i]>0? mat[i]/(max-min):0;//0.001*mat[i]/(max-min);
    }
}

double* getConvolutionMasks(const char* conv_file, int rows, int columns, int count)
{
    FILE *file;
    int i, j;
    char tmp[BUFF], *tmp1;
    double* masks;

    file = fopen(conv_file,"r");
    if(file==NULL){
        perror("Convolution masks file open error");
        return NULL;
    }

    masks = (double*) calloc(count*rows*columns, sizeof(double));
    if(masks==NULL){
        perror("Masks memory allocation error");
        fclose(file);
        return NULL;
    }

    for(j=0;j<count;j++)
    {
        i=0;

        while(1) 
        {
            tmp[i] = (char)fgetc(file);

            if(tmp[i] != EOF && tmp[i] != '#'){
                if(tmp[i] != ' ' && tmp[i] != '\n'){
                    i++;
                }
            }else{
                break;
            }
        };

        if(tmp[i] == EOF){
            return masks;
        }

        tmp[i] = '\0';

        tmp1 = strtok(tmp, ",");

        i=0;
        while (tmp1 != NULL)
        {
            masks[j*rows*columns + i++] = atof(tmp1);// assume correct data
            tmp1 = strtok(NULL, ",");
        }
    }
}

void setInLayer(convolution_layer* layer, double* arr)
{
    if(layer==NULL || arr==NULL) return;

    layer->in_arr = arr;
}

void setOutLayer(convolution_layer* layer, double* arr)
{
    if(layer==NULL || arr==NULL) return;

    layer->out_arr = arr;
}

void freeConvolutionLayer(convolution_layer* layer)
{
    free(layer->conf);
    free(layer->masks);
    free(layer);
}