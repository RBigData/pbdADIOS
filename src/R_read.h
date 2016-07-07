#ifndef __R_READ__
#define __R_READ__

SEXP dump_var (SEXP R_adios_fp,
               SEXP R_varname);

SEXP R_read(SEXP R_adios_path,
            SEXP R_varname,
            SEXP R_comm,
            SEXP R_adios_rank);
#endif