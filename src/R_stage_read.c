#include "R_bpls.h"
#include "R_dump.h"
#include "R_read.h"
#include "R_stage_read.h"
#include "finalizer.h"


/** 
 *  Get slice for each node
 */
void slice(uint64_t length, uint64_t *s, uint64_t *e, int rank, int mpisize)
{
    uint64_t start = 0;
    uint64_t end = 0;
    uint64_t rem = length % mpisize;

    start = length/mpisize * rank;
    end = length/mpisize * (rank+1);
    *s = start;
    *e = end;
    
    /* If our MPI size is greater
       than the number of y dimensions,
       then read the whole thing. */
    if (mpisize > length) {
        *e = length;
        *s = 0;
        return;
    }
    if (end > length) {
        end = length;
        *e = end;
        return;
    }
    if (rank == mpisize-1) {
        end += rem;
        *e = end;
    }

    *e -= *s;
}

/**
 * Read variable at the current step. 
 */
SEXP R_stage_read(SEXP R_adios_file_ptr,
                  SEXP R_varname,
                  SEXP R_nvars,
                  SEXP R_comm,
                  SEXP R_p,
                  SEXP R_adios_rank)
{
    ADIOS_FILE  *fp = R_ExternalPtrAddr(R_adios_file_ptr);
    int nvars = asInteger(R_nvars);   //number of variables to read
    //MPI_Comm comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);
    uint64_t rank = (uint64_t) asInteger(R_adios_rank);
    uint64_t p = (uint64_t) asInteger(R_p);
    
    SEXP R_vec = PROTECT(allocVector(VECSXP, nvars));
    SEXP list_names = PROTECT(allocVector(STRSXP, nvars));

    void *data_vec[nvars];   // store data pointers
    ADIOS_SELECTION *sel_vec[nvars];  // store selection pointers
    ADIOS_VARINFO *vi_vec[nvars];   // store ADIOS_VARINFO pointers
    int nelems_vec[nvars];     // store the number of elements in each variable

    int i, status;

    // schedule read
    for(i=0; i<nvars; i++) {
        nelems_vec[i] = schedule_stage_read (fp, 
                                             CHAR(asChar(VECTOR_ELT(R_varname,i))),
                                             &data_vec[i],
                                             &sel_vec[i],
                                             &vi_vec[i],
                                             p,
                                             rank);
        if(nelems_vec[i] < 0){
            return R_NilValue;
        }
    }

    // perform read
    status = adios_perform_reads (fp, 1); // blocking read performed here
    if (status < 0) {
        REprintf("Error when reading variable. errno=%d : %s \n", adios_errno, adios_errmsg());

        for(i=0; i<nvars; i++) {
            adios_free_varinfo(vi_vec[i]);
            Free(sel_vec[i]);
            Free(data_vec[i]);
        }
        return R_NilValue;
    }
    
    // Copy data into R memory
    for(i=0; i<nvars; i++) {
        SEXP R_temp_var;
        SEXP R_vi;
        SEXP R_data;

        newRptr(vi_vec[i], R_vi, finalizer0);
        newRptr(data_vec[i], R_data, finalizer0);

        R_temp_var = copy_read(R_vi, 
                               ScalarInteger(nelems_vec[i]),
                               R_data);

        SET_VECTOR_ELT(R_vec, i, R_temp_var);
        SET_STRING_ELT(list_names, i, asChar(VECTOR_ELT(R_varname,i)));

        UNPROTECT(2);
        // free memory
        adios_free_varinfo(vi_vec[i]);
        Free(sel_vec[i]);
        Free(data_vec[i]);

    }
    
    // set list attributes
    setAttrib(R_vec, R_NamesSymbol, list_names);
    
    // free memory
    UNPROTECT(2);

    return R_vec;
}

/**
 * Schedule read.
 */
int schedule_stage_read (ADIOS_FILE * fp, 
                         const char *varname,
                         void ** data,
                         ADIOS_SELECTION ** sel,
                         ADIOS_VARINFO ** vi,
                         uint64_t p,   //number of ranks
                         uint64_t rank)
{
    int     i, j;             // loop vars
    uint64_t nelems;         // number of elements to read
    int elemsize;            // size in bytes of one element
    int  status; 
    uint64_t *istart, *icount;

    // Inquiry about a variable. 
    *vi = adios_inq_var (fp, varname);
    if (!(*vi)) {
        REprintf("Error: %s\n", adios_errmsg());
        return -1;
    }

    // allocate memory for istart and icount
    if((*vi)->ndim == 0) {
        istart = (uint64_t *)malloc(sizeof(uint64_t));
        icount = (uint64_t *)malloc(sizeof(uint64_t));
        istart[0] = 0;
        icount[0] = 1;
    }else {
        istart = (uint64_t *)malloc((*vi)->ndim * sizeof(uint64_t));
        icount = (uint64_t *)malloc((*vi)->ndim * sizeof(uint64_t));
    }

    // assign values to istart and icount
    uint64_t N = 0;
    uint64_t pos = 0; 
    for (i=0; i<(*vi)->ndim; i++) {
        istart[i] = 0;
        icount[i] = (*vi)->dims[i];
        if(N < icount[i]) {
            N = icount[i];
            pos = i;
        }
    }
    slice(N, &istart[pos], &icount[pos], rank, p);
    
    // count the total number of elements
    nelems = 1;
    for (j=0; j<(*vi)->ndim; j++) {
        nelems *= icount[j];
    }

    if (getTypeInfo((*vi)->type, &elemsize)) {
        REprintf("Adios type %d (%s) not supported in bpls. var=%s\n", 
                (*vi)->type, adios_type_to_string((*vi)->type), varname);
        return -1;
    }
    // special case: string. Need to use different elemsize
    if ((*vi)->type == adios_string) {
        if ((*vi)->value)
            elemsize = strlen((*vi)->value)+1;
    }

    // allocate data array
    *data = (void *) malloc (nelems*elemsize+8); // +8 for just to be sure

    // read a slice finally
    *sel = adios_selection_boundingbox ((*vi)->ndim, istart, icount);
    status = adios_schedule_read_byid (fp, *sel, (*vi)->varid, 0, 1, *data); 


    if (status < 0) {
        REprintf("Error when scheduling variable %s for reading. errno=%d : %s \n", varname, adios_errno, adios_errmsg());
        adios_free_varinfo(*vi);
        Free(*sel);
        Free(*data);
        return -1;
    }

    // free allocated memory
    Free(istart);
    Free(icount);

    return nelems;
}
