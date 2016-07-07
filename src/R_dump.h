#ifndef __R_DUMP__
#define __R_DUMP__

#define MAX_DIMS 16
#define MAX_MASKS 10
#define MAX_BUFFERSIZE (10*1024*1024)

int getTypeInfo( enum ADIOS_DATATYPES adiosvartype, int* elemsize);

SEXP readVar(SEXP R_adios_fp, 
             SEXP R_adios_var_info, 
             SEXP R_name, 
             SEXP R_timed);

SEXP dump_vars (SEXP R_adios_fp);

SEXP R_dump(SEXP R_adios_path,
            SEXP R_comm,
            SEXP R_adios_rank);

#endif