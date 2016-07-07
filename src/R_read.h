#ifndef __R_READ__
#define __R_READ__

SEXP read_var(SEXP R_adios_fp, 
             SEXP R_adios_var_info, 
             SEXP R_name, 
             SEXP R_timed,
             SEXP R_start,
             SEXP R_count);

SEXP dump_var (SEXP R_adios_fp,
               SEXP R_varname,
               SEXP R_start,
               SEXP R_count);

SEXP R_read(SEXP R_adios_path,
            SEXP R_varname,
            SEXP R_start,
            SEXP R_count,
            SEXP R_comm,
            SEXP R_adios_rank);

#endif