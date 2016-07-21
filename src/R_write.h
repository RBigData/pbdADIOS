#ifndef __R_WRITE__
#define __R_WRITE__

SEXP R_create(SEXP R_groupname,
              SEXP R_buffersize,
              SEXP R_comm);

SEXP R_write(SEXP R_filename,
             SEXP R_group,
             SEXP R_groupname,
             SEXP R_nvars,          // number of vars
             SEXP R_varname_list,   // var names
             SEXP R_var_list,       // var values
             SEXP R_varlength_list, // length of var values
             SEXP R_comm,
             SEXP R_size,
             SEXP R_adios_rank);

#endif