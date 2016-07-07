#include "R_bpls.h"
#include "R_dump.h"
#include "R_read.h"

/** 
 *  Finalizer that only clears R pointer
 */
static void finalizer0(SEXP Rptr)
{
    void *ptr = (void *) R_ExternalPtrAddr(Rptr);
    if (NULL == ptr) {
        return;
    } else {
        R_ClearExternalPtr(Rptr);
    }
}

/**
 * Read a variable. 
 * If start and count is not specified, a whole list is returned.
 */
SEXP R_read(SEXP R_adios_path,
            SEXP R_varname,
            SEXP R_comm,
            SEXP R_adios_rank)
{
    ADIOS_FILE  *fp;
    int status;
    const char *path = CHARPT(R_adios_path, 0);
    MPI_Comm comm;
    comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);
    int rank = asInteger(R_adios_rank);
    SEXP R_vec;

    status = adios_read_init_method (ADIOS_READ_METHOD_BP, comm, "verbose=2");
    if (status) {
        REprintf("Error: %s\n", adios_errmsg());
        exit(6);
    }

    // open the BP file
    fp = adios_read_open_file (path, ADIOS_READ_METHOD_BP, comm); 
    if (fp == NULL) {
        exit(7);
    }

    SEXP R_adios_fp;
    newRptr(fp, R_adios_fp, finalizer0);

    if(!rank)
        R_vec = dump_var (R_adios_fp, R_varname);
    
    adios_read_close (fp);
    adios_read_finalize_method(ADIOS_READ_METHOD_BP);

    UNPROTECT(1);
    return R_vec;
}

/**
 * Dump a variable. If the start and count is not specified, read all values by default.
 */
SEXP dump_var (SEXP R_adios_fp,
               SEXP R_varname)
{
    ADIOS_FILE * fp = R_ExternalPtrAddr(R_adios_fp);
    const char *varname = CHARPT(R_varname, 0);
    ADIOS_VARINFO *vi; 
    enum ADIOS_DATATYPES vartype;
    int     i, j, n;             // loop vars

    int     retval;
    int     nNames; // number of vars + attrs
    bool    timed;  // variable has multiple timesteps

    // check the if the var is in the bp file
    retval = -1;
    nNames = fp->nvars;
    for (n=0; n<nNames; n++) {
        if(strcmp (varname,fp->var_namelist[n]) == 0) {
            retval = n;
            break;
        }   
    }

    if(retval == -1) {
        REprintf("Error: the variable doesn't exist in the bp file.\n");
        return R_NilValue;
    }

    // Inquiry about a variable. 
    vi = adios_inq_var (fp, fp->var_namelist[retval]);
    if (!vi) {
        REprintf("Error: %s\n", adios_errmsg());
    }

    SEXP vec;
    SEXP R_adios_var_info;
    vartype = vi->type;
    timed = (vi->nsteps > 1);
    newRptr(vi, R_adios_var_info, finalizer0);

    vec = readVar(R_adios_fp, R_adios_var_info, mkString(fp->var_namelist[retval]), ScalarInteger(timed));

    UNPROTECT(1);
    /* Free ADIOS_VARINFO */
    adios_free_varinfo(vi);

    return vec;
} 
