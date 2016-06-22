#include "R_adios.h"

// Do I need to include stdint etc here to use uint64_t?

/**
 *  R wrapper of ADIOS read API
 *  https://github.com/ornladios/ADIOS/blob/master/src/public/adios_read_v2.h
 */

/**
 *  Finalizer that frees memory and clears R pointer
 */
static void finalizer(SEXP Rptr)
{
    void *ptr = (void *) R_ExternalPtrAddr(Rptr);
    if (NULL == ptr) {
        R_debug_print("finalizer: Nothing to finalize\n");
        return;
    } else {
        R_debug_print("finalizer: About to free: %p ...\n", ptr);
        Free(ptr);
        R_debug_print("finalizer: Freed %p.\n", ptr);
        R_ClearExternalPtr(Rptr);
        R_debug_print("finalizer: %p Cleared Rptr.\n", ptr);
    }
}

/** 
 *  Finalizer that only clears R pointer
 */
static void finalizer0(SEXP Rptr)
{
    void *ptr = (void *) R_ExternalPtrAddr(Rptr);
    if (NULL == ptr) {
        R_debug_print("finalizer0: Nothing to finalize\n");
        return;
    } else {
        R_debug_print("finalizer0: Freed by ADIOS %p. Only clear.\n", ptr);
        R_ClearExternalPtr(Rptr);
        R_debug_print("finalizer0: %p Cleared Rptr.\n", ptr);
    }
}

/**
 *  ADIOS_READ_METHOD lookup table
 */
int read_method_hash(const char *search_str)
{
    typedef struct read_method_table {
        const char *method_name;
        int val;
    } adios_rmt;
    adios_rmt table[] = {
        { "ADIOS_READ_METHOD_BP"          , 0},
        { "ADIOS_READ_METHOD_BP_AGGREGATE", 1},
        { "ADIOS_READ_METHOD_DATASPACES"  , 3},
        { "ADIOS_READ_METHOD_DIMES"       , 4},
        { "ADIOS_READ_METHOD_FLEXPATH"    , 5},
    };

    for (adios_rmt *i = table; i->method_name != NULL; i++) {
        if (strcmp(i->method_name, search_str) == 0) {
            return i->val;
        }
    }
    return -EINVAL;
} 

/**
 *  ADIOS_LOCKMODE lookup table
 */                       
int lock_mode_hash(const char *search_str)
{
    typedef struct lock_mode_table {
        const char *method_name;
        int val;
    } adios_lm;
    adios_lm table[] = {
        { "ADIOS_LOCKMODE_NONE"   , 0},
        { "ADIOS_LOCKMODE_CURRENT", 1},
        { "ADIOS_LOCKMODE_ALL"    , 2},
    };

    for (adios_lm *i = table; i->method_name != NULL; i++) {
        if (strcmp(i->method_name, search_str) == 0) {
            return i->val;
        }
    }
    return -EINVAL;
}

/**
 *  Initialize a reading method before opening a file/stream with using the method.
 */
SEXP R_adios_read_init_method(SEXP R_adios_read_method,
                              SEXP R_comm,
                              SEXP R_params)
{
    SEXP ret;
    PROTECT(ret = allocVector(INTSXP, 1));
    
    int read_method_value = 1111; //init dummy value
    const char *method_name = CHARPT(R_adios_read_method, 0); //Passing Char pointer to "read_method_hash" function.       
    read_method_value = read_method_hash(method_name); //Calling read_method_hash function

    MPI_Comm comm;
    comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);

    const char *parameters = CHARPT(R_params, 0);
    
    INT(ret) = adios_read_init_method(read_method_value, 
                                      comm, 
                                      parameters);  

    UNPROTECT(1);
    return ret;
} 

/**
 *  Open an adios file/stream as a stream.
 *  Only one step at a time can be read. The list of variables will change when
 *  advancing the step if the writing application writes different variables at
 *  different times.
 */
SEXP R_adios_read_open(SEXP R_filename, 
                       SEXP R_adios_read_method, 
                       SEXP R_comm,
                       SEXP R_adios_lockmode, 
                       SEXP R_timeout_sec)
{
    const char *filename = CHARPT(R_filename, 0);
    MPI_Comm comm;
    comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);

    int read_method_value = 1111; //init dummy value                           
    const char *read_method_name = CHARPT(R_adios_read_method, 0); //Passing Char pointer to "read_method_hash" function.
    read_method_value = read_method_hash(read_method_name); //Calling read_method_hash function    

    int lock_mode_value = 1111; //init dummy value     
    const char *lock_method_name = CHARPT(R_adios_lockmode, 0); //Passing Char pointer to "lock_mode" function.                    
    lock_mode_value = lock_mode_hash(lock_method_name); //Calling read_method_hash function 

    float timeout_sec;
    //R does NOT have any objects that are in "float" (single precision) representations 
    //double check FLOAT vs DOUBLE in R. Here cast double* to float*
    timeout_sec= (float)asReal(R_timeout_sec); 

    //return type is ADIOS_FILE *
    ADIOS_FILE *adios_file_ptr; 
    SEXP R_adios_file_ptr;

    // pointer to an ADIOS_FILE struct
    adios_file_ptr  = adios_read_open(filename, 
                                      read_method_value, 
                                      comm,
                                      lock_mode_value, 
                                      timeout_sec);
    newRptr(adios_file_ptr, R_adios_file_ptr, finalizer0);
    R_debug_print("R_adios_read_open address: %p\n",
             (void *)R_ExternalPtrAddr(R_adios_file_ptr));
    UNPROTECT(1);
    return R_adios_file_ptr;
}

/** 
 *  Inquiry about a variable.
 *  This function does not read anything from the file but processes info
 *  already in memory after fopen.
 */
SEXP R_adios_inq_var(SEXP R_adios_file_ptr, 
                     SEXP R_adios_varname)
{
    ADIOS_FILE * fp;
    fp = R_ExternalPtrAddr(R_adios_file_ptr);

    const char *adios_varname = CHARPT(R_adios_varname, 0);

    ADIOS_VARINFO *adios_var_info;
    SEXP R_adios_var_info;

    adios_var_info = adios_inq_var(fp,
                                   adios_varname);
    newRptr(adios_var_info, R_adios_var_info, finalizer0);
    R_debug_print("R_adios_inq_var address: %p\n",
             (void *)R_ExternalPtrAddr(R_adios_var_info));
    UNPROTECT(1);
    return R_adios_var_info;   
}

/** Free memory used by an ADIOS_VARINFO struct */
SEXP R_adios_free_varinfo (SEXP R_adios_var_info)
{
    ADIOS_VARINFO *adios_var_info;
    adios_var_info = R_ExternalPtrAddr(R_adios_var_info);
    adios_free_varinfo(adios_var_info);

    return R_NilValue;
}

/** Get the block-decomposition of the variable about how it is stored in 
 *  the file or stream. The decomposition information are recorded in the
 *  metadata, so no extra file access is necessary after adios_fopen() for 
 *  this operation. The result is stored in the array of 
 *  ADIOS_VARBLOCK structs under varinfo.blocks. 
 */
SEXP R_adios_inq_var_blockinfo(SEXP R_adios_file_ptr, 
                               SEXP R_adios_var_info)
{
    SEXP ret;
    PROTECT(ret = allocVector(INTSXP, 1));

    ADIOS_FILE *fp;
    fp = R_ExternalPtrAddr(R_adios_file_ptr);

    ADIOS_VARINFO *adios_var_info;
    adios_var_info = R_ExternalPtrAddr(R_adios_var_info);

    INT(ret) = adios_inq_var_blockinfo(fp, 
                                       adios_var_info);

    UNPROTECT(1);
    return ret;
}

/** Get number of dimensions */
SEXP R_custom_inq_var_ndim(SEXP R_adios_var_info)
{
    ADIOS_VARINFO *adios_var_info;
    adios_var_info = R_ExternalPtrAddr(R_adios_var_info);

    SEXP R_custom_inq_var_ndim_val = PROTECT(allocVector(INTSXP, 1));
    INT(R_custom_inq_var_ndim_val) = adios_var_info -> ndim; 
    R_debug_print("In C ndim=%d \n", adios_var_info -> ndim);
    UNPROTECT(1);
    return R_custom_inq_var_ndim_val;
}

/** Get size of each dimension */
SEXP R_custom_inq_var_dims(SEXP R_adios_var_info)
{
    ADIOS_VARINFO *adios_var_info;
    adios_var_info = R_ExternalPtrAddr(R_adios_var_info);

    SEXP R_custom_inq_var_dims_val = PROTECT(allocVector(INTSXP,
                                     adios_var_info -> ndim));
    for(int i=0;i<adios_var_info -> ndim;i++){
        INTEGER (R_custom_inq_var_dims_val)[i] = adios_var_info -> dims[i];
    }
                                            
    UNPROTECT(1);
    return R_custom_inq_var_dims_val;
}

/** 
 *  adios_selection_bounding_box API.
 *  ADIOS_SELECTION object can be simply freed by free(), so adios_selection_delete is not implemented here
 *  https://github.com/ornladios/ADIOS/blob/master/src/public/adios_selection.h
 *  R reads double values here and then convert them into int and uint64_t
 */
SEXP R_adios_selection_boundingbox(SEXP R_adios_ndim, 
                                    SEXP R_adios_start,
                                    SEXP R_adios_count)
{
    // Make sure R_adios_ndim is interger
    int ndim = asInteger(R_adios_ndim);
    double* start = REAL(R_adios_start);
    double* count = REAL(R_adios_count);

    //Malloc 
    uint64_t *start_adios =  malloc( (ndim) * sizeof(uint64_t));
    uint64_t *count_adios =  malloc( (ndim) * sizeof(uint64_t));
    //Copy from start to start_adios and count to count_adios
    for (int pos = 0; pos < ndim; pos++) { 
        start_adios[pos] = (uint64_t)start[pos];
        count_adios[pos] = (uint64_t)count[pos];
    }

    ADIOS_SELECTION *adios_selection;
    SEXP R_adios_selection;

    adios_selection = adios_selection_boundingbox(ndim, 
                                                  start_adios, 
                                                  count_adios);
    newRptr(adios_selection, R_adios_selection, finalizer);
    UNPROTECT(1);

    // free memory
    Free(start_adios);
    Free(count_adios);
    return R_adios_selection;
}

/**
 *  Schedule reading a variable (slice) from the file.
 *  You need to call adios_perform_reads() to do the reading.
 *  Return: pointer to the memory to hold dsata of the variable.
 */
SEXP R_adios_schedule_read(SEXP R_adios_var_info, 
                           SEXP R_adios_start,
                           SEXP R_adios_count, 
                           SEXP R_adios_file_ptr,
                           SEXP R_adios_selection, 
                           SEXP R_adios_varname,
                           SEXP R_adios_from_steps, 
                           SEXP R_adios_nsteps)
{
    ADIOS_VARINFO *adios_var_info;
    adios_var_info = R_ExternalPtrAddr(R_adios_var_info);
                  
    int ndim = adios_var_info->ndim;
    double* start = REAL(R_adios_start);
    double* count = REAL(R_adios_count);

    //Malloc 
    uint64_t *start_adios =  malloc( (ndim) * sizeof(uint64_t));
    uint64_t *count_adios =  malloc( (ndim) * sizeof(uint64_t));
    //Copy from start to start_adios and count to count_adios
    for (int pos = 0; pos < ndim; pos++) { 
        start_adios[pos] = (uint64_t)start[pos];
        count_adios[pos] = (uint64_t)count[pos];
    }

    // get the memory size of one data element of an adios type, may ask Nobert this function?
    int datasize = adios_type_size(adios_var_info->type, adios_var_info->value);

    ADIOS_FILE * fp;
    fp = R_ExternalPtrAddr(R_adios_file_ptr);

    ADIOS_SELECTION *adios_selection;
    adios_selection = R_ExternalPtrAddr(R_adios_selection);

    const char *varname = CHARPT(R_adios_varname, 0);

    int from_steps = asInteger(R_adios_from_steps);
    int nsteps = asInteger(R_adios_nsteps);

    SEXP R_adios_data;
    void *adios_data;
    uint64_t ndata = 1;
    for (int dim = 0; dim < ndim; dim++)
        ndata *= count[dim];
    
    adios_data = malloc(ndata * datasize);
    adios_schedule_read(fp, 
                        adios_selection, 
                        varname, 
                        from_steps,
                        nsteps,
                        adios_data); 
    newRptr(adios_data, R_adios_data, finalizer);
    R_debug_print("R_adios_schedule_read address: %p\n",
                (void *)R_ExternalPtrAddr(R_adios_data));
    UNPROTECT(1);

    // free memory
    Free(start_adios);
    Free(count_adios);

    return R_adios_data;
}

/**
 *  Let ADIOS perform the scheduled reads
 */
SEXP R_adios_perform_reads(SEXP R_adios_file_ptr, 
                           SEXP R_adios_blocking)
{
    SEXP ret;
    PROTECT(ret = allocVector(INTSXP, 1));
    
    ADIOS_FILE *fp;
    fp = R_ExternalPtrAddr(R_adios_file_ptr);

    int blocking = asInteger(R_adios_blocking);
    
    INT(ret) = adios_perform_reads(fp, 
                                   blocking);
    UNPROTECT(1);
    return ret;
}

/**
 * Copy the scheduled reads from C object to R object
 */
SEXP R_custom_data_access(SEXP R_adios_data, 
                          SEXP R_adios_selection, 
                          SEXP R_adios_var_info)
{
    void *adios_data;
    adios_data = R_ExternalPtrAddr(R_adios_data);

    ADIOS_SELECTION *adios_selection;
    adios_selection = R_ExternalPtrAddr(R_adios_selection);

    ADIOS_VARINFO *adios_var_info;
    adios_var_info = R_ExternalPtrAddr(R_adios_var_info);

    // should we use uint64_t for the number of elements
    uint64_t num_element = 1;

    enum ADIOS_DATATYPES data_type;
    data_type = adios_var_info->type;

    SEXP R_custom_data_access_val; 

    for (int pos = 0; pos < adios_var_info->ndim; pos++) {
        //Assume we only use the ADIOS_SELECTION_BOUNDINGBOX method
        num_element = num_element * adios_selection->u.bb.count[pos];
    }

    //cast to specific datatypes (consider other datatypes later, i.e. string, complex)
    switch(data_type) {
        case adios_integer:
            R_custom_data_access_val = PROTECT(allocVector(INTSXP, num_element));
            int *data_int = (int *) adios_data;
            for(int i=0;i<num_element;i++){
                INTEGER(R_custom_data_access_val)[i] = *(data_int + i);
            }
            UNPROTECT(1);
            break;
        case adios_real:
            R_custom_data_access_val = PROTECT(allocVector(REALSXP, num_element));
            float *data_float = (float *) adios_data;   
            for(int i=0;i<num_element;i++){
                REAL(R_custom_data_access_val)[i] = *(data_float + i);
            }
            UNPROTECT(1);
            break;
        case adios_double:
            R_custom_data_access_val = PROTECT(allocVector(REALSXP, num_element));
            double *data_double = (double *) adios_data;   
            for(int i=0;i<num_element;i++){
                REAL(R_custom_data_access_val)[i] = *(data_double + i);
            }
            UNPROTECT(1);
            break;
        case adios_string:
            R_custom_data_access_val = PROTECT(allocVector(STRSXP, num_element));
            char *data_string = (char *) adios_data;   
            for(int i=0;i<num_element;i++){
                SET_STRING_ELT(R_custom_data_access_val, i, mkChar(data_string + i));
            }
            UNPROTECT(1);
            break;
        default:
            error("Error found in R_custom_data_access\n");
    }
    return R_custom_data_access_val; 
}

/**
 * Advance the current step of a stream. For files opened as file, stepping has no effect.
 */
SEXP R_adios_advance_step(SEXP R_adios_file_ptr, 
                          SEXP R_adios_last, 
                          SEXP R_adios_timeout_sec)
{
    SEXP ret;
    PROTECT(ret = allocVector(INTSXP, 1));
    
    ADIOS_FILE *fp;
    fp = R_ExternalPtrAddr(R_adios_file_ptr);
    
    int last;
    last = asInteger(R_adios_last);
    
    float timeout_sec;
    timeout_sec = (float) asReal(R_adios_timeout_sec);
    
    INT(ret) = adios_advance_step(fp, 
                                  last, 
                                  timeout_sec);
    UNPROTECT(1);
    return ret;
}

/**
 *  Close an adios file.
 *  It will free the content of the underlying data structures and the fp pointer itself.
 */
SEXP R_adios_read_close(SEXP R_adios_file_ptr)
{
    SEXP ret;
    PROTECT(ret = allocVector(INTSXP, 1));

    ADIOS_FILE * fp;
    fp = R_ExternalPtrAddr(R_adios_file_ptr);
    INT(ret) = adios_read_close(fp);

    return ret;
}

/**
 *  Finalize the selected method. Required for all methods that are initialized. 
 */
SEXP R_adios_read_finalize_method(SEXP R_adios_read_method)
{
    SEXP ret;
    PROTECT(ret = allocVector(INTSXP, 1));

    enum ADIOS_READ_METHOD read_method_value=1111; //init dummy value                          
    const char *method_name = CHARPT(R_adios_read_method, 0); //Passing Char pointer to "read_method_hash" function.                                    
    read_method_value = read_method_hash(method_name); //Calling read_method\ hash function  

    INT(ret) = adios_read_finalize_method(read_method_value);

    return ret;
}

/**
 *  Get error number
 */
SEXP R_adios_errno()
{
    SEXP R_adios_errno_val = PROTECT(allocVector(INTSXP, 1));
    INT(R_adios_errno_val) = adios_errno;
    R_debug_print("In C ndim=%d \n", adios_errno);
    UNPROTECT(1);
    return R_adios_errno_val;
}

