#ifndef H_CONV
#define H_CONV

#define NELEMS(x)  (int)(sizeof(x) / sizeof((x)[0]))
#define BUFF_BIG 200
#define BUFF_SMALL 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

void doConvolution(double*, double*, int, int, int);

#endif //H_CONV