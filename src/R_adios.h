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

// for Writer
// typedef struct {
//  int64_t       m_adios_group;
//  int64_t       m_adios_file;
//} m_adios_file_group;


#ifdef DEBUG
# define R_debug_print(...) Rprintf(__VA_ARGS__)
#else
# define R_debug_print(...)
#endif


#define INT(x) INTEGER(x)[0]
#define newRptr(ptr,Rptr,fin) PROTECT(Rptr = R_MakeExternalPtr(ptr, R_NilValue, R_NilValue)); R_RegisterCFinalizerEx(Rptr, fin, TRUE)
 /* newRptr(already_allocated_C_pointer, R_pointer_to_be_made_for_it, finalizer) */

#define CHARPT(x,i)     ((char*)CHAR(STRING_ELT(x,i)))

/* ADIOS utility functions. */
//SEXP R_adios_init(SEXP R_filename, SEXP R_comm);
//SEXP R_adios_finalize(SEXP R_comm_rank);

SEXP AsInt(int x);

/* ADIOS file functions. */
//SEXP R_adios_open(SEXP R_filename, SEXP R_comm);
//SEXP R_adios_close(SEXP R_adios_handle);

/* For internal buffeer management and to construct the group index table.*/
//SEXP R_adios_group_size(SEXP R_adios_handle, SEXP R_adios_groupsize,
//      SEXP R_adios_totalsize);
/* ADIOS read functions. */
//SEXP R_adios_read(SEXP R_adios_handle, SEXP R_var_name, SEXP R_var,
//      SEXP R_adios_buf_size);

/* New read method for Streaming */

int read_method_hash(const char *search_str);
int lock_mode_hash(const char *search_str);
SEXP R_adios_read_init_method(SEXP R_adios_read_method, SEXP R_comm, SEXP R_params);
SEXP R_adios_read_open(SEXP R_filename, SEXP R_adios_read_method, SEXP R_comm,
           SEXP R_adios_lockmode, SEXP R_timeoout_sec);
SEXP R_adios_read_close(SEXP R_adios_file_ptr);
SEXP R_adios_read_finalize_method(SEXP R_adios_read_method);

SEXP R_adios_inq_var(SEXP R_adios_file_ptr, SEXP R_adios_varname);
SEXP R_adios_inq_var_blockinfo(SEXP R_adios_file_ptr, SEXP R_adios_var_info);
SEXP R_adios_selection_bounding_box(SEXP R_adios_ndim, SEXP R_adios_start,
            SEXP R_adios_count);

SEXP R_adios_schedule_read(SEXP R_adios_varinfo, SEXP R_adios_start,
         SEXP R_adios_count, SEXP R_adios_file_ptr,
         SEXP R_adios_selection, SEXP R_adios_varname,
         SEXP R_adios_from_steps, SEXP R_adios_nsteps);   

SEXP R_adios_perform_reads(SEXP R_adios_file_ptr, SEXP R_adios_blocking);

SEXP R_adios_advance_step(SEXP R_adios_file_ptr, SEXP R_adios_last,
        SEXP R_adios_timeout_sec);

SEXP R_adios_errno();


/* ADIOS writer methods */

SEXP R_adios_init_noxml(SEXP R_comm);
SEXP R_adios_allocate_buffer(SEXP R_buffer_size);
SEXP R_adios_declare_group(SEXP R_adios_group_name, SEXP R_adios_time_index);
SEXP R_adios_select_method(SEXP R_m_adios_group, SEXP R_adios_method, SEXP R_adios_params, SEXP R_adios_base_path);
SEXP R_adios_define_var(SEXP R_m_adios_group, SEXP R_adios_varname, SEXP R_adios_path, SEXP R_adios_local_dim, SEXP R_adios_global_dim, SEXP R_adios_local_offset);
SEXP R_adios_open(SEXP R_adios_group_name, SEXP R_adios_file_name, SEXP R_adios_mode, SEXP R_comm);

SEXP R_adios_group_size(SEXP R_m_adios_file, SEXP R_adios_group_size);
SEXP R_adios_write(SEXP R_m_adios_file, SEXP R_adios_var_name, SEXP R_adios_var);

SEXP R_adios_close(SEXP R_m_adios_file);
SEXP R_adios_finalize(SEXP R_comm_rank);


#endif


