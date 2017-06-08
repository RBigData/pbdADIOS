#ifndef __R_ADIOS__
#define __R_ADIOS__

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

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#ifdef DEBUG
# define R_debug_print(...) Rprintf(__VA_ARGS__)
#else
# define R_debug_print(...)
#endif


#define INT(x) INTEGER(x)[0]
#define newRptr(ptr,Rptr,fin) PROTECT(Rptr = R_MakeExternalPtr(ptr, R_NilValue, R_NilValue)); R_RegisterCFinalizerEx(Rptr, fin, TRUE)
 /* newRptr(already_allocated_C_pointer, R_pointer_to_be_made_for_it, finalizer) */
#define CHARPT(x,i)     ((char*)CHAR(STRING_ELT(x,i)))

/* ADIOS read API */
int read_method_hash(const char *search_str);
int lock_mode_hash(const char *search_str);


/* ADIOS get attributes API */
struct ATTR{
    enum ADIOS_DATATYPES type;
    int size;
    void *data;
};


/* ADIOS write API */
int buffer_alloc_when_hash(const char *search_str);
int adios_flag_hash(const char *search_str);
int adios_datatypes_hash(const char *search_str);



#endif
