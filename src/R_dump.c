#include "R_bpls.h"
#include "R_dump.h"

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
 * R wrapper of dump. 
 * A list of variable values is returned.
 */
SEXP R_dump(SEXP R_adios_path,
            SEXP R_comm,
            SEXP R_adios_rank)
{
    ADIOS_FILE  *fp;
    int status;
    const char *path = CHARPT(R_adios_path, 0);
    MPI_Comm comm;
    comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);
    int rank = asInteger(R_adios_rank);
    SEXP R_vec = R_NilValue;

    status = adios_read_init_method (ADIOS_READ_METHOD_BP, comm, "verbose=2");
    if (status) {
        REprintf("Error: %s\n", adios_errmsg());
        //exit(6);
        return R_NilValue;
    }

    // open the BP file
    fp = adios_read_open_file (path, ADIOS_READ_METHOD_BP, comm); 
    if (fp == NULL) {
        REprintf("Error: %s\n", adios_errmsg());
        //exit(7);
        return R_NilValue;
    }

    SEXP R_adios_fp;
    newRptr(fp, R_adios_fp, finalizer0);

    if(!rank)
        R_vec = dump_vars (R_adios_fp);
    
    adios_read_close (fp);
    adios_read_finalize_method(ADIOS_READ_METHOD_BP);

    UNPROTECT(1);
    return R_vec;
}

/**
 * dump vars
 */
SEXP dump_vars (SEXP R_adios_fp)
{
    ADIOS_FILE * fp = R_ExternalPtrAddr(R_adios_fp);

    ADIOS_VARINFO *vi; 
    ADIOS_VARINFO **vis; 
    //enum ADIOS_DATATYPES vartype;
    int     n;             // loop vars

    int     nNames; // number of vars + attrs
    bool    timed;  // variable has multiple timesteps

    nNames = fp->nvars;

    SEXP vec = PROTECT(allocVector(VECSXP, nNames));
    SEXP list_names = PROTECT(allocVector(STRSXP, nNames));

    vis = (ADIOS_VARINFO **) malloc (nNames * sizeof (ADIOS_VARINFO*));
    if (vis == NULL) {
        REprintf("Error: could not allocate %d elements\n", nNames);
        return R_NilValue;
    }

    //names = fp-var_namelist
    for (n=0; n<nNames; n++) {
        vis[n] = adios_inq_var (fp, fp->var_namelist[n]);
        if (!vis[n]) {
            REprintf("Error: %s\n", adios_errmsg());
        }
    }

    /* VARIABLES */
    for (n=0; n<nNames; n++) {
        SEXP R_temp_var;
        SEXP R_adios_var_info;

        vi = vis[n];
        //vartype = vi->type;
        timed = (vi->nsteps > 1);
        newRptr(vi, R_adios_var_info, finalizer0);

        R_temp_var = readVar(R_adios_fp, R_adios_var_info, mkString(fp->var_namelist[n]), ScalarInteger(timed));
        SET_VECTOR_ELT(vec, n, R_temp_var);
        SET_STRING_ELT(list_names, n,  mkChar(fp->var_namelist[n]));
        UNPROTECT(1);
    }

    // set list attributes
    setAttrib(vec, R_NamesSymbol, list_names);

    /* Free ADIOS_VARINFOs */
    for (n=0; n<nNames; n++) {
        adios_free_varinfo(vis[n]);
    }

    UNPROTECT(2);
    return vec;
} 

/** 
 * Read data of a variable
 * Return: 0: vector of variable values
 */
SEXP readVar(SEXP R_adios_fp, 
             SEXP R_adios_var_info, 
             SEXP R_name, 
             SEXP R_timed)
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
            out = PROTECT(allocVector(INTSXP, nelems));
            break;
        case adios_byte:
            out = PROTECT(allocVector(INTSXP, nelems));
            break;

        case adios_string:
            out = PROTECT(allocVector(STRSXP, nelems));
            break;
        case adios_string_array:
            // we expect one elemet of the array here
            out = PROTECT(allocVector(STRSXP, nelems));
            break;

        case adios_unsigned_short:  
            out = PROTECT(allocVector(INTSXP, nelems));
            break;
        case adios_short:
            out = PROTECT(allocVector(INTSXP, nelems));
            break;

        case adios_unsigned_integer:
            out = PROTECT(allocVector(INTSXP, nelems));
            break;
        case adios_integer:    
            out = PROTECT(allocVector(INTSXP, nelems));
            break;

        case adios_unsigned_long:
            out = PROTECT(allocVector(REALSXP, nelems));
            break;
        case adios_long:
            out = PROTECT(allocVector(REALSXP, nelems));
            break;   

        case adios_real:
            out = PROTECT(allocVector(REALSXP, nelems));
            break;
        case adios_double:
            out = PROTECT(allocVector(REALSXP, nelems));
            break;

        case adios_complex:  
            out = PROTECT(allocVector(REALSXP, 2*nelems));
            break;

        case adios_double_complex:
            out = PROTECT(allocVector(REALSXP, 2*nelems));
            break;

        case adios_long_double:
            out = PROTECT(allocVector(REALSXP, nelems));
            break;

        default:
            out = R_NilValue;
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
                while (item < steps) {
                    INTEGER(out)[pos++] = ((uint8_t *)data)[item++];
                }
                break;
            case adios_byte:
                while (item < steps) {
                    INTEGER(out)[pos++] = ((int8_t *)data)[item++];
                }
                break;

            case adios_string:
                while (item < steps) {
                    SET_STRING_ELT(out, pos++, mkChar((char *)data + item));
                    item++;
                }
                break;
            case adios_string_array:
                // we expect one elemet of the array here
                while (item < steps) {
                    SET_STRING_ELT(out, pos++, mkChar(*((char **)data + item)));
                    item++;
                }
                break;

            case adios_unsigned_short:  
                while (item < steps) {
                    INTEGER(out)[pos++] = ((uint16_t *)data)[item++];
                }
                break;
            case adios_short:
                while (item < steps) {
                    INTEGER(out)[pos++] = ((int16_t *)data)[item++];
                }
                break;

            case adios_unsigned_integer:
                while (item < steps) {
                    INTEGER(out)[pos++] = ((uint32_t *)data)[item++];
                }
                break;
            case adios_integer:    
                while (item < steps) {
                    INTEGER(out)[pos++] = ((int32_t *)data)[item++];
                }
                break;

            case adios_unsigned_long:
                while (item < steps) {
                    REAL(out)[pos++] = ((uint64_t *)data)[item++];
                }
                break;
            case adios_long:
                while (item < steps) {
                    REAL(out)[pos++] = ((int64_t *)data)[item++];
                }
                break;   

            case adios_real:
                while (item < steps) {
                    REAL(out)[pos++] = ((float *)data)[item++];
                }
                break;
            case adios_double:
                while (item < steps) {
                    REAL(out)[pos++] = ((double *)data)[item++];
                }
                break;

            case adios_complex:  
                while (item < steps) {
                    REAL(out)[pos++] = ((float *)data)[2*item];
                    REAL(out)[pos++] = ((float *)data)[2*item+1];
                    item++;
                }
                //Rprintf("(%g,i%g)", ((float *) data)[2*item], ((float *) data)[2*item+1]);
                break;

            case adios_double_complex:
                while (item < steps) {
                    REAL(out)[pos++] = ((double *)data)[2*item];
                    REAL(out)[pos++] = ((double *)data)[2*item+1];
                    item++;
                }
                //Rprintf("(%g,i%g)", ((double *) data)[2*item], ((double *) data)[2*item+1]);
                break;

            case adios_long_double:
                while (item < steps) {
                    REAL(out)[pos++] = ((long double *)data)[item++];
                }
                break;
               
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

/**
 * Get datatype size
 */
int getTypeInfo( enum ADIOS_DATATYPES adiosvartype, int* elemsize)
{
    switch(adiosvartype) {
        case adios_unsigned_byte:
            *elemsize = 1;
            break;
        case adios_byte:
            *elemsize = 1;
            break;
        case adios_string:
            *elemsize = 1;
            break;

        case adios_unsigned_short:  
            *elemsize = 2;
            break;
        case adios_short:
            *elemsize = 2;
            break;

        case adios_unsigned_integer:
            *elemsize = 4;
            break;
        case adios_integer:    
            *elemsize = 4;
            break;

        case adios_unsigned_long:
            *elemsize = 8;
            break;
        case adios_long:        
            *elemsize = 8;
            break;

        case adios_real:
            *elemsize = 4;
            break;

        case adios_double:
            *elemsize = 8;
            break;

        case adios_complex:  
            *elemsize = 8;
            break;

        case adios_double_complex:
            *elemsize = 16;
            break;

        case adios_long_double:
            *elemsize = 16;
            break;
        default:
            return 1;
    }
    return 0;
}