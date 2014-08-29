#!/bin/bash

#module swap PE-intel PE-gnu
#module swap gnu gnu/4.8.2
#module swap intel intel/12.1.233
#module swap mpt mpt/2.04
#module unload mpt/2.01
#module load r/3.0.1 

MPI_INCLUDE=-I${OMPI_DIR}/include
MPI_LIBS=-L${OMPI_DIR}/lib

R_INCLUDE=-I${R_HOME}/include
R_LIBS=-L${R_HOME}/lib
ADIOS_DIR=${ADIOS_DIR}_wrappers
ADIOS_INCLUDE=-I${ADIOS_DIR}/include
ADIOS_LIBS=`${ADIOS_DIR}/bin/adios_config -lr`

#ADIOS_LIBS_STATIC=${ADIOS_DIR}/lib
#export LD_LIBRARY_PATH=$ADIOS_LIBS_STATIC:$LD_LIBRARY_PATH

GCC_LIBS_64=-L${GCC}/lib64/
GCC_LIBS=-L${GCC}/lib

# Change R version later
#CC="gcc -std=gnu99"
CC=mpicc

CPPFLAGS="$MPI_INCLUDE -DMPI2 -DOPENMPI"
CFLAGS="-w -fpic"
CXXFLAGS="-g -O2"
FCFLAGS="-g -O2"
LDLAGS="$MPI_LIBS -lompi -lutil -lpthread $ADIOS_LIBS"

#rm *.o 
#rm *.so


#$CC -std=gnu99 $R_INCLUDE -DNDEBUG $MPI_INCLUDE -DMPI2 -DOPENMPI -I/usr/local/include   -w -fpic  -g -O2  -c R_adios_util.c -o R_adios_util.o
#$CC -std=gnu99 $R_INCLUDE -DNDEBUG $MPI_INCLUDE -DMPI2 -DOPENMPI -I/usr/local/include   -w -fpic  -g -O2  -c R_adios_file.c -o R_adios_file.o
echo $CC -std=gnu99 $R_INCLUDE -DNDEBUG $MPI_INCLUDE -DMPI2 -DOPENMPI -I/usr/local/include $ADIOS_INCLUDE -w -fpic  -g -O2  -c R_adios_read.c -o R_adios_read.o
$CC -std=gnu99 $R_INCLUDE -DNDEBUG $MPI_INCLUDE -DMPI2 -DOPENMPI -I/usr/local/include $ADIOS_INCLUDE -w -fpic  -g -O2  -c R_adios_read.c -o R_adios_read.o
#$CC -std=gnu99 $R_INCLUDE -DNDEBUG $MPI_INCLUDE -DMPI2 -DOPENMPI -I/usr/local/include   -w -fpic  -g -O2  -c R_adios_write.c -o R_adios_write.o
#$CC -std=gnu99 $R_INCLUDE -DNDEBUG $MPI_INCLUDE -DMPI2 -DOPENMPI -I/usr/local/include   -w -fpic  -g -O2  -c R_adios_group.c -o R_adios_group.o


#$CC -std=gnu99 -shared $GCC_LIBS_64 $GCC_LIBS -L/usr/local/lib64 -o pbdADIOS_stream.so R_adios_util.o R_adios_file.o R_adios_read.o R_adios_write.o R_adios_group.o $ADIOS_LIBS_STATIC/libadios.a $ADIOS_LIBS_STATIC/libadios_nompi.a $ADIOS_LIBS_STATIC/libadiosread.a $ADIOS_LIBS_STATIC/libadiosread_nompi.a $ADIOS_LIBS_STATIC/libmxml.a $MPI_LIBS -lmpi -lutil -lpthread $R_LIBS -lR

$CC -std=gnu99 -shared $GCC_LIBS_64 $GCC_LIBS -L/usr/local/lib64 -o pbdADIOS_stream.so R_adios_read.o $ADIOS_LIBS $R_LIBS -lR  


