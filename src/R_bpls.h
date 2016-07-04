#ifndef __R_BPLS__
#define __R_BPLS__

#include <stdio.h>
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
#define newRptr(ptr,Rptr,fin) PROTECT(Rptr = R_MakeExternalPtr(ptr, R_NilValue, R_NilValue)); R_RegisterCFinalizerEx(Rptr, fin, TRUE)
 /* newRptr(already_allocated_C_pointer, R_pointer_to_be_made_for_it, finalizer) */
#define CHARPT(x,i)     ((char*)CHAR(STRING_ELT(x,i)))

void mergeLists(int nV, char **listV, int nA, char **listA, char **mlist, bool *isVar);
int print_data(void *data, int item, enum ADIOS_DATATYPES adiosvartype);
int doList_group (ADIOS_FILE *fp);

SEXP R_bpls(SEXP R_adios_path,
            SEXP R_comm,
            SEXP R_adios_rank);

#endif