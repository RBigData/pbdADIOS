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

#include "adios.h"
#include "adios_read.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

// for Writer
typedef struct {
  int64_t       m_adios_group;
  int64_t       m_adios_file;
} R_adios_file_group;



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


/* New read method for Streaming*/

int read_method_hash(const char *search_str);
int lock_mode_hash(const char *search_str);
SEXP R_adios_read_init_method(SEXP R_adios_read_method, SEXP R_comm, SEXP R_params);
SEXP R_adios_read_open(SEXP R_filename, SEXP R_adios_read_method, SEXP R_comm, SEXP R_adios_lockmode, SEXP R_timeoout_sec);
SEXP R_adios_read_close(SEXP R_adios_file_ptr);
SEXP R_adios_read_finalize_method(SEXP R_adios_read_method);

SEXP R_adios_inq_var(SEXP R_adios_file_ptr, SEXP R_adios_varname);
SEXP R_adios_inq_var_blockinfo(SEXP R_adios_file_ptr, SEXP R_adios_var_info);
SEXP R_adios_selection_bounding_box(SEXP R_adios_ndim, SEXP R_adios_start, SEXP R_adios_count);


//SEXP R_adios_schedule_read(SEXP R_adios_ndim, SEXP R_adios_start,
//                           SEXP R_adios_count, SEXP R_adios_datatype, SEXP R_adios_file_ptr, SEXP R_adios_selection, SEXP R_adios_varname, SEXP R_adios_from_steps, SEXP R_adios_nsteps);

SEXP R_adios_schedule_read(SEXP R_adios_varinfo, SEXP R_adios_start, SEXP R_adios_count, SEXP R_adios_file_ptr, SEXP R_adios_selection, SEXP R_adios_varname, SEXP R_adios_from_steps, SEXP R_adios_nsteps);   


SEXP R_adios_perform_reads(SEXP R_adios_file_ptr, SEXP R_adios_blocking);

SEXP R_adios_advance_step(SEXP R_adios_file_ptr, SEXP R_adios_last, SEXP R_adios_timeout_sec);

/* ADIOS writer methods */
// GO: below needs to be SEXP'd
SEXP R_adios_write_open(MPI_Comm, char *,char *,char *,char *);
//communicator, groupname, transportmethod, filename, mode  : Return "R_adios_file_group" struct  

SEXP R_adios_write_close(MPI_Comm, int, char **, int *, char **, char **, char **, R_adios_file_group *, void **);

SEXP R_adios_finalize(SEXP R_comm_rank);


#endif


