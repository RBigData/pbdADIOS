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
    uint64_t start_t[MAX_DIMS], count_t[MAX_DIMS]; // processed <0 values in start/count
    uint64_t s[MAX_DIMS], c[MAX_DIMS]; // for block reading of smaller chunks
    int tdims;               // number of dimensions including time
    int tidx;                // 0 or 1 to account for time dimension
    uint64_t nelems;         // number of elements to read
    int elemsize;            // size in bytes of one element
    uint64_t st, ct;
    void *data;
    uint64_t sum;           // working var to sum up things
    int  maxreadn;          // max number of elements to read once up to a limit (10MB of data)
    int  actualreadn;       // our decision how much to read at once
    int  readn[MAX_DIMS];   // how big chunk to read in in each dimension?
    int  status;            
    bool incdim;            // used in incremental reading in
    ADIOS_SELECTION * sel;  // boundnig box to read
    int pos;                // index for copy data to R memory
    SEXP out;               // store the variable values
    
    int  istart[MAX_DIMS], icount[MAX_DIMS];
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
    }

    if (getTypeInfo(vi->type, &elemsize)) {
        REprintf("Adios type %d (%s) not supported in bpls. var=%s\n", 
                vi->type, adios_type_to_string(vi->type), name);
        return R_NilValue;
    }

    // create the counter arrays with the appropriate lengths
    // transfer start and count arrays to format dependent arrays

    nelems = 1;
    tidx = 0;

    if (timed) {
        if (istart[0] < 0)  // negative index means last-|index|
            st = vi->nsteps+istart[0];
        else
            st = istart[0];
        if (icount[0] < 0)  // negative index means last-|index|+1-start
            ct = vi->nsteps+icount[0]+1-st;
        else
            ct = icount[0];

        if (verbose>2) 
            Rprintf("    j=0, st=%" PRIu64 " ct=%" PRIu64 "\n", st, ct);

        start_t[0] = st;
        count_t[0] = ct;
        nelems *= ct;
        if (verbose>1) 
            Rprintf("    s[0]=%" PRIu64 ", c[0]=%" PRIu64 ", n=%" PRIu64 "\n",
                    start_t[0], count_t[0], nelems);
        
        tidx = 1;
    }
    tdims = vi->ndim + tidx;

    for (j=0; j<vi->ndim; j++) {
        if (istart[j+tidx] < 0)  // negative index means last-|index|
            st = vi->dims[j]+istart[j+tidx];
        else
            st = istart[j+tidx];
        if (icount[j+tidx] < 0)  // negative index means last-|index|+1-start
            ct = vi->dims[j]+icount[j+tidx]+1-st;
        else
            ct = icount[j+tidx];

        if (verbose>2) 
            Rprintf("    j=%d, st=%" PRIu64 " ct=%" PRIu64 "\n", j+tidx, st, ct);

        start_t[j+tidx] = st;
        count_t[j+tidx] = ct;
        nelems *= ct;
        if (verbose>1) 
            Rprintf("    s[%d]=%" PRIu64 ", c[%d]=%" PRIu64 ", n=%" PRIu64 "\n",
                    j+tidx, start_t[j+tidx], j+tidx, count_t[j+tidx], nelems);
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

        //case adios_complex:           
        //case adios_double_complex:
        //case adios_long_double: // do not know how to print
           
        default:
            break;
    }

    maxreadn = MAX_BUFFERSIZE/elemsize;
    if (nelems < maxreadn)
        maxreadn = nelems;

    // special case: string. Need to use different elemsize
    if (vi->type == adios_string) {
        if (vi->value)
            elemsize = strlen(vi->value)+1;
        maxreadn = elemsize;
    }

    // allocate data array
    data = (void *) malloc (maxreadn*elemsize+8); // +8 for just to be sure

    // determine strategy how to read in:
    //  - at once
    //  - loop over 1st dimension
    //  - loop over 1st & 2nd dimension
    //  - etc
    if (verbose>1) Rprintf("Read size strategy:\n");
    sum = (uint64_t) 1;
    actualreadn = (uint64_t) 1;
    for (i=tdims-1; i>=0; i--) {
        if (sum >= (uint64_t) maxreadn) {
            readn[i] = 1;
        } else {
            readn[i] = maxreadn / (int)sum; // sum is small for 4 bytes here
            // this may be over the max count for this dimension
            if (readn[i] > count_t[i]) 
                readn[i] = count_t[i];
        }
        if (verbose>1) Rprintf("    dim %d: read %d elements\n", i, readn[i]);
        sum = sum * (uint64_t) count_t[i];
        actualreadn = actualreadn * readn[i];
    }
    if (verbose>1) Rprintf("    read %d elements at once, %" PRId64 " in total (nelems=%" PRId64 ")\n", actualreadn, sum, nelems);


    // init s and c
    for (j=0; j<tdims; j++) {
        s[j]=start_t[j];
        c[j]=readn[j];
    }

    // read until read all 'nelems' elements
    sum = 0;
    pos = 0;
    while (sum < nelems) {
        // how many elements do we read in next?
        actualreadn = 1;
        for (j=0; j<tdims; j++) 
            actualreadn *= c[j];

        // read a slice finally
        sel = adios_selection_boundingbox (vi->ndim, s+tidx, c+tidx);
        if (timed) {
            status = adios_schedule_read_byid (fp, sel, vi->varid, s[0], c[0], data); 
        } else {
            status = adios_schedule_read_byid (fp, sel, vi->varid, 0, 1, data); 
        }

        if (status < 0) {
            REprintf("Error when scheduling variable %s for reading. errno=%d : %s \n", name, adios_errno, adios_errmsg());
            Free(data);
            return R_NilValue;
        }

        status = adios_perform_reads (fp, 1); // blocking read performed here
        if (status < 0) {
            REprintf("Error when reading variable %s. errno=%d : %s \n", name, adios_errno, adios_errmsg());
            Free(data);
            return R_NilValue;
        }

        /**
         * start copying data to R memory
         */
        //print_dataset(data, vi->type, s, c, tdims, ndigits_dims); 
        int pi, item, steps;

        // init current indices
        steps = 1;
        for (pi=0; pi<tdims; pi++) {
            steps *= c[pi];
        }

        item = 0; // index to *data 
        // loop through each data item and print value

        switch(vi->type) {
            case adios_unsigned_byte:
            case adios_byte:
            case adios_string:
                while (item < steps) {
                    SET_STRING_ELT(out, pos++, mkChar((char *)data + item));
                    item++;
                }
                break;

            case adios_unsigned_short:  
            case adios_short:
            case adios_unsigned_integer:
            case adios_integer:    
                while (item < steps) {
                    INTEGER(out)[pos++] = ((int *)data)[item++];
                }
                break;

            case adios_unsigned_long:
            case adios_long:        
            case adios_real:
            case adios_double:
                while (item < steps) {
                    REAL(out)[pos++] = ((double *)data)[item++];
                }
                break;

            //case adios_complex:           
            //case adios_double_complex:
            //case adios_long_double: // do not know how to print
               
            default:
                break;
        }
        /**
         * end copying data to R memory
         */

        // prepare for next read
        sum += actualreadn;
        incdim=true; // largest dim should be increased 
        for (j=tdims-1; j>=0; j--) {
            if (incdim) {
                if (s[j]+c[j] == start_t[j]+count_t[j]) {
                    // reached the end of this dimension
                    s[j] = start_t[j];
                    c[j] = readn[j];
                    incdim = true; // next smaller dim can increase too
                } else {
                    // move up in this dimension up to total count
                    s[j] += readn[j];
                    if (s[j]+c[j] > start_t[j]+count_t[j]) {
                        // do not reach over the limit
                        c[j] = start_t[j]+count_t[j]-s[j];
                    }
                    incdim = false;
                }
            }
        }
    } // end while sum < nelems

    Free(data);
    UNPROTECT(1);
    return out;
}
