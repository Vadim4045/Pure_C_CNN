#Makefile command file for Simple ANN

ann: main.o ann.o layer.o data.o
	gcc -O3 -g -o ann main.o ann.o layer.o data.o -lm

main.o: main.c
	gcc -c main.c -o main.o

ann.o: simpleANN.c
	gcc -c simpleANN.c -o ann.o

layer.o: annLayer.c
	gcc -O3 -c annLayer.c -o layer.o

data.o: dataImport.c
	gcc -c dataImport.c -o data.o

clean: 
	rm -f *.o ann