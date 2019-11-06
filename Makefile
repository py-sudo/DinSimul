# Author: Peiyi Guan
# Date: July 2019
# Description: Thread simulator

SRC = simul.c error.c
OBJ = simul.o error.o
HDR = args.h error.h

TRG = simul

${TRG}: ${OBJ}
	gcc ${OBJ} -pthread -lc -o ${TRG}

simul.o: simul.c ${HDR}
	gcc -c -pthread simul.c

error.o: error.c ${HDR}
	gcc -c error.c

clean:
	/bin/rm ${OBJ} ${TRG}

tar:
	tar cf DinSimul.tar ${SRC} ${HDR} Makefile
	../sendit DinSimul.tar

test:
	echo ./simul -L .9 -M 0.10 -T 100000> tests
	./simul -L .9 -M 0.10 -T 100000 >> tests
	echo >> tests
	echo ./simul -L .9 -M 0.10 -T 10000>> tests
	./simul -L .9 -M 0.10 -T 100000 >> tests
	echo >> tests
	echo ./simul -L 0.0 -M 1.0 >> tests
	./simul -L 0.0 -M 1.0 >> tests
	echo >> tests
	echo ./simul -L 1.0 -M 0.0 -T 10000 >> tests
	./simul -L 1.0 -M 0.0 -T 10000 >> tests
	echo >> tests
	echo ./simul -L 0.90 -M 0.10 -T 10000 >> tests
	./simul -L 0.90 -M 0.10 -T 10000 >> tests
	echo >> tests
	echo ./simul -L 0.5 -M 0.5 -T 10000 >> tests
	./simul -L 0.5 -M 0.5 -T 10000 >> tests
	echo >> tests
	echo ./simul -L 0.1 -M 0.5 -T 10000 -N 20 >> tests
	./simul -L 0.1 -M 0.5 -T 10000 -N 20 >> tests
