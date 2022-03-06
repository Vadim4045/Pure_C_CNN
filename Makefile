#Makefile command file for Pure_C_CNN

cnn: main.o ann.o layer.o data.o conv.o config.o
	gcc  -O3 -g -o cnn main.o ann.o layer.o data.o conv.o config.o -lm

main.o: main.c
	gcc -c main.c -o main.o

config.o: config_parser.c
	gcc -O3 -c config_parser.c -o config.o

conv.o: convolutionLayer.c
	gcc -O3 -c convolutionLayer.c -o conv.o

ann.o: simpleANN.c
	gcc -c simpleANN.c -o ann.o

layer.o: annLayer.c
	gcc  -O3 -c annLayer.c -o layer.o

data.o: dataImport.c
	gcc -c dataImport.c -o data.o

clean: 
	rm -f *.o cnn