
#ifndef __R_ADIOS__
#define __R_ADIOS__

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <mpi.h>
#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>
#include "adios/src/public/adios.h"


/* Obtain character pointers. */
#define CHARPT(x,i)     ((char*)CHAR(STRING_ELT(x,i)))

/* ADIOS utility functions. */
SEXP R_adios_init(SEXP R_filename, SEXP R_comm);
SEXP R_adios_finalize(SEXP R_comm_rank);
SEXP AsInt(int x);

/* ADIOS file functions. */
SEXP R_adios_open(SEXP R_filename, SEXP R_comm);
SEXP R_adios_close(SEXP R_adios_handle);

/* For internal buffeer management and to construct the group index table.*/
SEXP R_adios_group_size(SEXP R_adios_handle, SEXP R_adios_groupsize, SEXP R_adios_totalsize);

/* ADIOS read functions. */
SEXP R_adios_read(SEXP R_adios_handle, SEXP R_var_name, SEXP R_var, SEXP R_adios_buf_size);

/* ADIOS write functions. */
SEXP R_adios_write(SEXP R_adios_handle, SEXP R_var_name, SEXP R_var);

#endif

