#include "R_bpls.h"
#include "R_dump.h"
#include "R_read.h"

/**
 *  This version read a variable at onece
 */ 

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
            SEXP R_start,
            SEXP R_count,
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
        R_vec = dump_var (R_adios_fp, R_varname, R_start, R_count);
    
    adios_read_close (fp);
    adios_read_finalize_method(ADIOS_READ_METHOD_BP);

    UNPROTECT(1);
    return R_vec;
}

/**
 * Dump a variable. If the start and count is not specified, read all values by default.
 */
SEXP dump_var (SEXP R_adios_fp,
               SEXP R_varname,
               SEXP R_start,
               SEXP R_count)
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

    vec = read_var(R_adios_fp, 
                   R_adios_var_info, 
                   mkString(fp->var_namelist[retval]), 
                   ScalarInteger(timed),
                   R_start,
                   R_count);

    UNPROTECT(1);
    /* Free ADIOS_VARINFO */
    adios_free_varinfo(vi);

    return vec;
} 

/** 
 * Read data of a variable
 * Return: variable values
 */
SEXP read_var(SEXP R_adios_fp, 
             SEXP R_adios_var_info, 
             SEXP R_name, 
             SEXP R_timed,
             SEXP R_start,
             SEXP R_count)
{
    ADIOS_FILE * fp = R_ExternalPtrAddr(R_adios_fp);
    ADIOS_VARINFO *vi = R_ExternalPtrAddr(R_adios_var_info);
    const char *name = CHARPT(R_name, 0);
    bool timed = asInteger(R_timed);

    int i,j;
    int tidx;                // 0 or 1 to account for time dimension
    uint64_t nelems;         // number of elements to read
    int elemsize;            // size in bytes of one element
    int item;
    void *data;
    int  status;            
    ADIOS_SELECTION * sel;  // boundnig box to read
    int pos;                // index for copy data to R memory
    SEXP out;               // store the variable values
    
    uint64_t istart[MAX_DIMS], icount[MAX_DIMS];
    int  verbose = 0;
    for (i=0; i<MAX_DIMS; i++) {
        istart[i]  = 0;
        icount[i]  = -1;  // read full var by default
    }

    // Check start and count. If they are not null, use them.
    if(INTEGER(R_start)[0] != -1) {
        // If the var is scalar, you don't need to specify start and count.
        if(vi->ndim == 0) {
            REprintf("The variable is scalar. You don't need to specify start and count.\n");
            return R_NilValue;
        }

        if(timed) {
            // check if the length of start matches ndim
            if(vi->ndim != (length(R_start) - 1)) {
                REprintf("Error: wrong start dims. \n");
                return R_NilValue;
            }
            // check if the step value is out of range.
            if((INTEGER(R_start)[0] < 0) || (INTEGER(R_start)[0] >= vi->nsteps)) {
                REprintf("Error: start %d out of bound. \n", INTEGER(R_start)[0]);
                return R_NilValue;
            }
            // check if the start value is out of range.
            for(i = 0; i < vi->ndim; i++) {
                if((INTEGER(R_start)[i+1] < 0) || (INTEGER(R_start)[i+1] >= vi->dims[i])) {
                    REprintf("Error: start %d out of bound. \n", INTEGER(R_start)[i+1]);
                    return R_NilValue;
                }
            }

            // assign start to istart
            for (i=0; i<vi->ndim+1; i++) {
                istart[i] = INTEGER(R_start)[i];
            }

        } else {
             // check if the length of start matches ndim
            if(vi->ndim != length(R_start)) {
                REprintf("Error: wrong start dims. \n");
                return R_NilValue;
            }
            // check if the start value is out of range.
            for(i = 0; i < vi->ndim; i++) {
                if((INTEGER(R_start)[i] < 0) || (INTEGER(R_start)[i] >= vi->dims[i])) {
                    REprintf("Error: start %d out of bound. \n", INTEGER(R_start)[i]);
                    return R_NilValue;
                }
            }

            // assign start to istart
            for (i=0; i<vi->ndim; i++) {
                istart[i] = INTEGER(R_start)[i];
            }
        }
    }

    if(INTEGER(R_count)[0] != -2) {
        // If the var is scalar, you don't need to specify start and count.
        if(vi->ndim == 0) {
            REprintf("The variable is scalar. You don't need to specify start and count.\n");
            return R_NilValue;
        }
        
        if(timed) {
            // check if the length of count matches ndim
            if(vi->ndim != (length(R_count) - 1)) {
                REprintf("Error: wrong count dims. \n");
                return R_NilValue;
            }
            // check if the step value is out of range.
            if((istart[0] + INTEGER(R_count)[0]) > vi->nsteps) {
                REprintf("Error: count %d out of bound. \n", INTEGER(R_count)[0]);
                return R_NilValue;
            }
            // check if the count value is out of range.
            for(i = 0; i < vi->ndim; i++) {
                if((istart[i+1] + INTEGER(R_count)[i+1]) > vi->dims[i]) {
                    REprintf("Error: count %d out of bound. \n", INTEGER(R_count)[i+1]);
                    return R_NilValue;
                }
            }

            // assign count to icount
            for (i=0; i<vi->ndim+1; i++) {
                icount[i] = INTEGER(R_count)[i];
            }

        } else {
             // check if the length of count matches ndim
            if(vi->ndim != length(R_count)) {
                REprintf("Error: wrong count dims. \n");
                return R_NilValue;
            }
            // check if the count value is out of range.
            for(i = 0; i < vi->ndim; i++) {
                if((istart[i] + INTEGER(R_count)[i]) > vi->dims[i]) {
                    REprintf("Error: count %d out of bound. \n", INTEGER(R_count)[i]);
                    return R_NilValue;
                }
            }

            // assign count to icount
            for (i=0; i<vi->ndim; i++) {
                icount[i] = INTEGER(R_count)[i];
            }
        }
    }else {
        // if the count is not specified, read all values
        if(timed) {
            icount[0] = vi->nsteps - istart[0];
            for (i=0; i<vi->ndim; i++)
                icount[i+1] = vi->dims[i+1] - istart[i+1];
        }else {
            for (i=0; i<vi->ndim; i++)
                icount[i] = vi->dims[i] - istart[i];
        }
    }

    if (getTypeInfo(vi->type, &elemsize)) {
        REprintf("Adios type %d (%s) not supported in bpls. var=%s\n", 
                vi->type, adios_type_to_string(vi->type), name);
        return R_NilValue;
    }

    // count the total number of elements
    nelems = 1;
    tidx = 0;
    if (timed) {
        nelems *= icount[0];
        tidx = 1;
    }

    for (j=0; j<vi->ndim; j++) {
        nelems *= icount[j+tidx];
    }

    if (verbose>1) {
        Rprintf(" total size of data to read = %" PRIu64 "\n", nelems*elemsize);
    }

    // Allocate R memory for the variable values
    switch(vi->type) {
        case adios_unsigned_byte:
        case adios_byte:
        case adios_string:
            out = PROTECT(allocVector(STRSXP, nelems));
            break;

        case adios_unsigned_short:  
        case adios_short:
        case adios_unsigned_integer:
        case adios_integer:    
            out = PROTECT(allocVector(INTSXP, nelems));
            break;

        case adios_unsigned_long:
        case adios_long:        
        case adios_real:
        case adios_double:
            out = PROTECT(allocVector(REALSXP, nelems));
            break;
           
        default:
            break;
    }

    // special case: string. Need to use different elemsize
    if (vi->type == adios_string) {
        if (vi->value)
            elemsize = strlen(vi->value)+1;
    }

    // allocate data array
    data = (void *) malloc (nelems*elemsize+8); // +8 for just to be sure

    // read a slice finally
    sel = adios_selection_boundingbox (vi->ndim, istart+tidx, icount+tidx);
    if (timed) {
        status = adios_schedule_read_byid (fp, sel, vi->varid, istart[0], icount[0], data); 
    } else {
        status = adios_schedule_read_byid (fp, sel, vi->varid, 0, 1, data); 
    }

    if (status < 0) {
        REprintf("Error when scheduling variable %s for reading. errno=%d : %s \n", name, adios_errno, adios_errmsg());
        Free(sel);
        Free(data);
        return R_NilValue;
    }

    status = adios_perform_reads (fp, 1); // blocking read performed here
    if (status < 0) {
        REprintf("Error when reading variable %s. errno=%d : %s \n", name, adios_errno, adios_errmsg());
        Free(sel);
        Free(data);
        return R_NilValue;
    }

    /**
     * start copying data to R memory
     */
    pos = 0;
    item = 0; // index to *data 
    switch(vi->type) {
        case adios_unsigned_byte:
        case adios_byte:
        case adios_string:
            while (item < nelems) {
                SET_STRING_ELT(out, pos++, mkChar((char *)data + item));
                item++;
            }
            break;

        case adios_unsigned_short:  
        case adios_short:
        case adios_unsigned_integer:
        case adios_integer:    
            while (item < nelems) {
                INTEGER(out)[pos++] = ((int *)data)[item++];
            }
            break;

        case adios_unsigned_long:
        case adios_long:        
        case adios_real:
        case adios_double:
            while (item < nelems) {
                REAL(out)[pos++] = ((double *)data)[item++];
            }
            break;
           
        default:
            break;
    }
    /**
     * end copying data to R memory
     */
        
    Free(sel);
    Free(data);
    UNPROTECT(1);
    return out;
}
