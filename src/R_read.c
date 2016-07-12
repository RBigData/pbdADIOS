#include "R_bpls.h"
#include "R_dump.h"
#include "R_read.h"

/**
 *  This version read multiple variables at onece
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
            SEXP R_nvars,
            SEXP R_comm,
            SEXP R_adios_rank)
{
    ADIOS_FILE  *fp;
    int status;
    const char *path = CHARPT(R_adios_path, 0);
    MPI_Comm comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);
    int rank = asInteger(R_adios_rank);
    int nvars = asInteger(R_nvars);   //number of variables to read
    int i;
    SEXP R_vec = PROTECT(allocVector(VECSXP, nvars));
    SEXP list_names = PROTECT(allocVector(STRSXP, nvars));
    //void *data_vec[nvars];   // store data pointers
    //ADIOS_SELECTION *sel_vec[nvars];  // store selection pointers
    //ADIOS_VARINFO *vi_vec[nvars];   // store ADIOS_VARINFO pointers
    int nelems_vec[nvars];     // store the number of elements in each variable

    void *data;
    ADIOS_SELECTION *sel;
    ADIOS_VARINFO *vi;

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

    // schedule read
    if(!rank) {
        int *start = INTEGER(VECTOR_ELT(R_start, 0));
        REprintf("The 1st value of start is %d\n", start[0]);

        for(i=0; i<nvars; i++) {
            /*nelems_vec[i] = schedule_read (fp, 
                                           CHAR(asChar(VECTOR_ELT(R_varname,i))),
                                           INTEGER(VECTOR_ELT(R_start, i)), 
                                           length(VECTOR_ELT(R_start, i)),
                                           INTEGER(VECTOR_ELT(R_count, i)),
                                           length(VECTOR_ELT(R_count, i)),
                                           data_vec[i],
                                           sel_vec[i],
                                           vi_vec[i]);*/
            nelems_vec[i] = schedule_read (fp, 
                                           CHAR(asChar(VECTOR_ELT(R_varname,i))),
                                           INTEGER(VECTOR_ELT(R_start, i)), 
                                           length(VECTOR_ELT(R_start, i)),
                                           INTEGER(VECTOR_ELT(R_count, i)),
                                           length(VECTOR_ELT(R_count, i)),
                                           data,
                                           sel,
                                           vi);

            if(nelems_vec[i] < 0){
                return R_NilValue;
            }
        }
        REprintf("end schedule read\n");
        REprintf("1st nelems is, %d \n", nelems_vec[0]);
    }

    // perform read
    status = adios_perform_reads (fp, 1); // blocking read performed here
    if (status < 0) {
        REprintf("Error when reading variable. errno=%d : %s \n", adios_errno, adios_errmsg());

        /*for(i=0; i<nvars; i++) {
            adios_free_varinfo(vi_vec[i]);
            Free(sel_vec[i]);
            Free(data_vec[i]);
        }*/
        return R_NilValue;
    }
    REprintf("end perform read\n");
    REprintf("8th data is, %d \n", ((int *)data)[8]);

    
    // Copy data into R memory
    for(i=0; i<nvars; i++) {
        SEXP R_temp_var;
        SEXP R_vi;
        SEXP R_data;

        //newRptr(vi_vec[i], R_vi, finalizer0);
        //newRptr(data_vec[i], R_data, finalizer0);

        newRptr(vi, R_vi, finalizer0);
        newRptr(data, R_data, finalizer0);

        //REprintf("Test read, %d \n", *((int *)data_vec[i]+8));

        R_temp_var = copy_read(R_vi, 
                               ScalarInteger(nelems_vec[i]),
                               R_data);

        REprintf("end copy read\n");

        SET_VECTOR_ELT(R_vec, i, R_temp_var);
        SET_STRING_ELT(list_names, i,  mkChar(fp->var_namelist[i]));

        UNPROTECT(2);
        // free memory
        //adios_free_varinfo(vi_vec[i]);
        //Free(sel_vec[i]);
        //Free(data_vec[i]);
    }

    // set list attributes
    setAttrib(R_vec, R_NamesSymbol, list_names);
    
    // free memory
    adios_read_close (fp);
    adios_read_finalize_method(ADIOS_READ_METHOD_BP);

    return R_vec;
}

/**
 * Schedule read. If the start and count is not specified, read all values by default.
 */
int schedule_read (ADIOS_FILE * fp, 
                  const char *varname,
                  int* start, 
                  int s_length,
                  int* count,
                  int c_length,
                  void *data,
                  ADIOS_SELECTION *sel,
                  ADIOS_VARINFO *vi)
{
    int     i, j, n;             // loop vars
    int     retval;
    int     nNames; // number of vars
    bool    timed;  // variable has multiple timesteps
    int tidx;                // 0 or 1 to account for time dimension
    uint64_t nelems;         // number of elements to read
    int elemsize;            // size in bytes of one element
    int  status;            

    // compute start and count
    uint64_t istart[MAX_DIMS], icount[MAX_DIMS];
    for (i=0; i<MAX_DIMS; i++) {
        istart[i]  = 0;
        icount[i]  = -1;  // read full var by default
    }

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
        return -1;
    }

    // Inquiry about a variable. 
    vi = adios_inq_var (fp, fp->var_namelist[retval]);
    if (!vi) {
        REprintf("Error: %s\n", adios_errmsg());
        return -1;
    }

    timed = (vi->nsteps > 1);
    
    REprintf("start[0] is %d. \n", start[0]);

    // Check start and count. If they are not null, use them.
    if(start[0] != -1) {
        // If the var is scalar, you don't need to specify start and count.
        if(vi->ndim == 0) {
            REprintf("The variable is scalar. You don't need to specify start and count.\n");
            return -1;
        }

        if(timed) {
            // check if the length of start matches ndim
            if(vi->ndim != (s_length - 1)) {
                REprintf("Error: wrong start dims. \n");
                REprintf("s_length is %d. \n", s_length);
                return -1;
            }
            // check if the step value is out of range.
            if((start[0] < 0) || (start[0] >= vi->nsteps)) {
                REprintf("Error: start %d out of bound. \n", start[0]);
                return -1;
            }
            // check if the start value is out of range.
            for(i = 0; i < vi->ndim; i++) {
                if((start[i+1] < 0) || (start[i+1] >= vi->dims[i])) {
                    REprintf("Error: start %d out of bound. \n", start[i+1]);
                    return -1;
                }
            }

            // assign start to istart
            for (i=0; i<vi->ndim+1; i++) {
                istart[i] = start[i];
            }

        } else {
             // check if the length of start matches ndim
            if(vi->ndim != s_length) {
                REprintf("Error: wrong start dims. \n");
                REprintf("s_length is %d. \n", s_length);
                REprintf("vi ndim is %d. \n", vi->ndim);
                return -1;
            }
            // check if the start value is out of range.
            for(i = 0; i < vi->ndim; i++) {
                if((start[i] < 0) || (start[i] >= vi->dims[i])) {
                    REprintf("Error: start %d out of bound. \n", start[i]);
                    return -1;
                }
            }

            // assign start to istart
            for (i=0; i<vi->ndim; i++) {
                istart[i] = start[i];
            }
        }
    }

    if(count[0] != -2) {
        // If the var is scalar, you don't need to specify start and count.
        if(vi->ndim == 0) {
            REprintf("The variable is scalar. You don't need to specify start and count.\n");
            return -1;
        }
        
        if(timed) {
            // check if the length of count matches ndim
            if(vi->ndim != (c_length - 1)) {
                REprintf("Error: wrong count dims. \n");
                return -1;
            }
            // check if the step value is out of range.
            if((istart[0] + count[0]) > vi->nsteps) {
                REprintf("Error: count %d out of bound. \n", count[0]);
                return -1;
            }
            // check if the count value is out of range.
            for(i = 0; i < vi->ndim; i++) {
                if((istart[i+1] + count[i+1]) > vi->dims[i]) {
                    REprintf("Error: count %d out of bound. \n", count[i+1]);
                    return -1;
                }
            }

            // assign count to icount
            for (i=0; i<vi->ndim+1; i++) {
                icount[i] = count[i];
            }

        } else {
             // check if the length of count matches ndim
            if(vi->ndim != c_length) {
                REprintf("Error: wrong count dims. \n");
                return -1;
            }
            // check if the count value is out of range.
            for(i = 0; i < vi->ndim; i++) {
                if((istart[i] + count[i]) > vi->dims[i]) {
                    REprintf("Error: count %d out of bound. \n", count[i]);
                    return -1;
                }
            }

            // assign count to icount
            for (i=0; i<vi->ndim; i++) {
                icount[i] = count[i];
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
                vi->type, adios_type_to_string(vi->type), varname);
        return -1;
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
        REprintf("Error when scheduling variable %s for reading. errno=%d : %s \n", varname, adios_errno, adios_errmsg());
        adios_free_varinfo(vi);
        Free(sel);
        Free(data);
        return -1;
    }

    return nelems;
} 

/** 
 * Copy data from C memory to R memory
 * Return: variable values
 */
SEXP copy_read (SEXP R_adios_var_info, 
                SEXP R_nelems, 
                SEXP R_data)
{
    int item;
    int pos;                // index for copy data to R memory
    SEXP out;               // store the variable values

    ADIOS_VARINFO *vi = R_ExternalPtrAddr(R_adios_var_info);
    int nelems = asInteger(R_nelems);
    void *data = R_ExternalPtrAddr(R_data);

    pos = 0;
    item = 0; // index for *data 
    // allocate memory and copy data to R memory
    switch(vi->type) {
        case adios_unsigned_byte:
        case adios_byte:
        case adios_string:
            out = PROTECT(allocVector(STRSXP, nelems));
            while (item < nelems) {
                SET_STRING_ELT(out, pos++, mkChar((char *)data + item));
                item++;
            }
            break;

        case adios_unsigned_short:  
        case adios_short:
        case adios_unsigned_integer:
        case adios_integer:    
            out = PROTECT(allocVector(INTSXP, nelems));
            while (item < nelems) {
                INTEGER(out)[pos++] = ((int *)data)[item++];
            }
            break;

        case adios_unsigned_long:
        case adios_long:        
        case adios_real:
        case adios_double:
            out = PROTECT(allocVector(REALSXP, nelems));
            while (item < nelems) {
                REAL(out)[pos++] = ((double *)data)[item++];
            }
            break;
           
        default:
            break;
    }

    return out;
}


