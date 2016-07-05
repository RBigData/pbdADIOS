#ifndef __R_DUMP__
#define __R_DUMP__

/*#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <mpi.h>

#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

#include <adios.h>
#include <adios_read.h>
#include <inttypes.h>
#include <stdbool.h>


#ifdef DMALLOC
#include "dmalloc.h"
#endif


#define INT(x) INTEGER(x)[0]
#define newRptr(ptr,Rptr,fin) PROTECT(Rptr = R_MakeExte-rnalPtr(ptr, R_NilValue, R_NilValue)); R_RegisterCFinalizerEx(Rptr, fin, TRUE)
 /* newRptr(already_allocated_C_pointer, R_pointer_to_be_made_for_it, finalizer) */
/*#define CHARPT(x,i)     ((char*)CHAR(STRING_ELT(x,i)))

void mergeLists(int nV, char **listV, int nA, char **listA, char **mlist, bool *isVar);
int print_data(void *data, int item, enum ADIOS_DATATYPES adiosvartype);
int doList_group (ADIOS_FILE *fp);
*/

#define MAX_DIMS 16
#define MAX_MASKS 10
#define MAX_BUFFERSIZE (10*1024*1024)

int getTypeInfo( enum ADIOS_DATATYPES adiosvartype, int* elemsize);

void print_endline(void);
int print_dataset(void *data, enum ADIOS_DATATYPES adiosvartype, 
        uint64_t *s, uint64_t *c, int tdims, int *ndigits);
int readVar(ADIOS_FILE *fp, ADIOS_VARINFO *vi, const char * name, bool timed);
int dump_vars (ADIOS_FILE *fp);

SEXP R_dump(SEXP R_adios_path,
            SEXP R_comm,
            SEXP R_adios_rank);

#endif