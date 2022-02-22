#include "convolution_layer.h"

typedef struct _conv_mask
{
    int position;
    int rows;
    int columns;
    double* mask;
    struct _conv_mask next;

}conv_mask;

conv_mask* getConvolutionMasks();
conv_mask* makeNewMask(int, int, int);
void freeMask(conv_mask*);
int getMasksCount(conv_mask*);



convolution_layer* newConvolutionLayer
(int in_rows, int in_columns, int mask_rows, int mask_columns, int step, int reduse)
{
    int tmpSize;

    convolution_layer* layer = (convolution_layer*)calloc(1,sizeof(convolution_layer));
    if(layer ==NULL) return NULL;

    layer->in_rows = in_rows;
    layer->in_columns = in_columns;
    layer->step = step;
    layer->reduse = reduse;

    layer->mask = getConvolutionMasks("convolution_set.conv", mask_rows, mask_columns);
    if(layer->mask ==NULL){
        free(layer);
        return NULL;
    }
    layer->mask_count = getMasksCount(layer->mask);

    layer->outSize = ((in_rows + mask_rows/2 + mask_rows/2)/step)*((in_columns + mask_columns/2 + mask_columns/2)/step);
    layer->intermediate = (double*) calloc(layer->outSize ,sizeof(double));
    if(layer->intermediate == NULL ){
        freeConvolutionLayer(layer);
        return NULL;
    }

    if(layer->reduse == 0){
        layer->out_arr = layer->intermediate;
        return layer;
    }

    layer->outSize = (((in_rows + mask_rows/2 + mask_rows/2)/step)/2)*(((in_columns + mask_columns/2 + mask_columns/2)/step)*2);
    layer->out_arr = (double*) calloc(layer->outSize ,sizeof(double));
    if(layer->out_arr == NULL ){
        freeConvolutionLayer(layer);
        return NULL;
    }

}

void doConvolution(convolution_layer* layer)
{
    int res_rows, res_columns, i, j, n, m, k=0;
    conv_mask* cur_mask = layer->mask;

    res_rows = (layer->in_rows + cur_mask->rows/2 + cur_mask->rows/2)/layer->step;
    res_columns = (layer->in_columns + cur_mask->columns/2 + cur_mask->columns/2)/layer->step;

    while (cur_mask != NULL)
    {
        for(i = -(cur_mask->rows)/2;i < layer->in_rows + (cur_mask->rows)/2;i+=layer->step){
            for(j = -(cur_mask->columns)/2;j < layer->in_columns + (cur_mask->columns)/2;j+=layer->step){

                if(i>=0 && j>=0 && i<res_rows && j<res_columns){
                    layer->intermediate[i*res_columns+j] = 0.0;
                }

                for(n=0;n<cur_mask->rows;n++){
                    for(m=0;m<cur_mask->columns;m++){
                        if(i+n>=0 && j+m>=0 && i+n<res_rows && j+m<res_columns){
                            layer->intermediate[(i+k)*res_columns+j]+= layer->in_arr[i*layer->in_columns+j] * cur_mask->mask[n*cur_mask->columns+m];
                        }
                    }
                }
                
            }
        }

        cur_mask = cur_mask->next;
        k += res_columns*res_rows;
    }

    if(layer->reduse == 0) return;
    
    k=0;
    res_rows /= 2;
    res_columns /= 2;

    while (cur_mask != NULL)
    {
        for(i = 0;i < layer->in_rows;i+=2){
            for(j = 0;j < layer->in_columns;j+=2){

                layer->out_arr[(i+k)*res_columns+j] = 0.0;

                for(n=0;n<2;n++){
                    for(m=0;m<2;m++){
                        if(i+n>=0 && j+m>=0 && i+n<res_rows && j+m<res_columns){
                            layer->out_arr[(i+k)*res_columns+j]+= layer->intermediate[i*layer->in_columns+j];
                        }
                    }
                }

                layer->out_arr[(i+k)*res_columns+j] /= 4.0;
                
            }
        }

        cur_mask = cur_mask->next;
        k += res_columns*res_rows;
    }
}

conv_mask* getConvolutionMasks(const char* conv_file, int rows, int columns)
{
    FILE *file;
    int i, count=0;
    char tmp[BUFF_BIG], *tmp1;
    conv_mask *first=NULL, *current;

    file = fopen(conv_file,"r");
    if(file==NULL){
        perror("Convolution masks file open error");
        return NULL;
    }

    while (1)
    {
        i=0;
        while(1) 
        {
            tmp[i] = (char)fgetc(file);

            if(tmp[i] != EOF && tmp[i] != '\n'){
                if(tmp[i] != ' '){
                    i++;
                }
            }else{
                break;
            }
        };

        if(tmp[i] == EOF) return first;
        if(tmp[0]=='#' || tmp[0] == '\n') continue;
        tmp[i] = '\0';


        if(current != NULL){
            current->next = makeNewMask(count++, rows, columns);
            current = current->next;
        } else current = makeNewMask(count++, rows, columns);  
        if(current == NULL){
            freeMask(first);
            fclose(file);
            return NULL;
        }
        if(first==NULL) current=first;

        tmp1 = strtok(tmp, ",");
        i=0;
        while (tmp1 != NULL)
        {
            current->mask[i++] = atof(tmp1)/10.0;// assume correct data
            tmp1 = strtok(NULL, ",");
        }
    }
}

conv_mask* makeNewMask(int position, int rows, int columns){
    conv_mask* mask = (conv_mask*) calloc(1,sizeof(conv_mask));
    if(mask==NULL){
        perror("Convolution mask memory allocation");
        return NULL;
    }

    mask->position = position;
    mask->rows = rows;
    mask->columns = columns;

    mask->mask = (double*)calloc(mask->rows*mask->columns, sizeof(double));
    if(mask->mask==NULL){
        freeMask(mask);
        perror("Convolution mask memory allocation(array)");
        return(NULL);
    }

    return mask;
}

void freeConvolutionLayer(convolution_layer* layer)
{
    freeMask(layer->mask);

    if(layer->intermediate != NULL) free(layer->intermediate);
    if(layer->out_arr != NULL && layer->out_arr != layer->intermediate)
        free(layer->out_arr);
    free(layer);
}

int getMasksCount(conv_mask* mask){
    if(mask->next != NULL) return getMasksCount(mask->next);
    else return mask->position;
}

void freeMask(conv_mask* mask){
    if(mask == NULL) return;

    freeMask(mask->next);

    if(mask->mask != NULL) free(mask->mask);
    free(mask);
}