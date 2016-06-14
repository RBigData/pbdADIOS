#include <stdint.h>
#include <inttypes.h>
#include "R_adios.h"

/* 
* R wrapper of ADIOS write API
* https://github.com/ornladios/ADIOS/blob/master/src/public/adios.h
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
 *  ADIOS_BUFFER_ALLOC_WHEN lookup table
 */
int buffer_alloc_when_hash(const char *search_str)
{
    typedef struct buffer_alloc_when_table{
        const char *method_name;
        int val;
    } adios_baw;
    adios_baw table[] = {
        { "ADIOS_BUFFER_ALLOC_UNKNOWN", 0},
        { "ADIOS_BUFFER_ALLOC_NOW"    , 1},
        { "ADIOS_BUFFER_ALLOC_LATER"  , 2},
    };

    for (adios_baw *i = table; i->method_name != NULL; i++) {
        if (strcmp(i->method_name, search_str) == 0) {
            return i->val;
        }
    }
    return -EINVAL;
} 

/**
 *  ADIOS_FLAG lookup table
 */
int adios_flag_hash(const char *search_str)
{
    typedef struct adios_flag_table{
        const char *method_name;
        int val;
    } adios_af;
    adios_af table[] = {
        { "adios_flag_unknown", 0},
        { "adios_flag_yes"    , 1},
        { "adios_flag_no"     , 2},
    };

    for (adios_af *i = table; i->method_name != NULL; i++) {
        if (strcmp(i->method_name, search_str) == 0) {
            return i->val;
        }
    }
    return -EINVAL;
} 

/**
 *  ADIOS_DATATYPES lookup table
 */
int adios_datatypes_hash(const char *search_str)
{
    typedef struct adios_datatypes_table{
        const char *method_name;
        int val;
    } adios_ad;
    adios_ad table[] = {
        { "adios_unknown"         , -1},

        { "adios_byte"            , 0},
        { "adios_short"           , 1},
        { "adios_integer"         , 2},
        { "adios_long"            , 4},

        { "adios_unsigned_byte"   , 50},
        { "adios_unsigned_short"  , 51},
        { "adios_unsigned_integer", 52},
        { "adios_unsigned_long"   , 54},

        { "adios_real"            , 5},
        { "adios_double"          , 6},
        { "adios_long_double"     , 7},

        { "adios_string"          , 9},
        { "adios_complex"         , 10},
        { "adios_double_complex " , 11},

        { "adios_string_array"    , 12},
    };

    for (adios_ad *i = table; i->method_name != NULL; i++) {
        if (strcmp(i->method_name, search_str) == 0) {
            return i->val;
        }
    }
    return -EINVAL;
} 

/**
 * ADIOS No-XML API's
 */
SEXP R_adios_init_noxml(SEXP R_comm)
{
    MPI_Comm comm;
    comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);
    
    adios_init_noxml(comm); //Calling adios_init_nomxl function 

    return(R_NilValue);
} 

/**
 *  To allocate ADIOS buffer OBSOLETE?
 *  adios_buffer_alloc_when - indicates when ADIOS buffer should be allocated. 
 *  The value can be ei- ther ADIOS_BUFFER_ALLOC_NOW or ADIOS_BUFFER_ALLOC_LATER.
 */
SEXP R_adios_allocate_buffer(SEXP R_adios_buffer_when, 
                             SEXP R_buffer_size)
{
    SEXP ret;
    PROTECT(ret = allocVector(INTSXP, 1));

    int buffer_when_value=1111; //init dummy value
    const char *buffer_when = CHARPT(R_adios_buffer_when, 0);
    buffer_when_value = buffer_alloc_when_hash(buffer_when); //Calling buffer_alloc_when_hash function

    uint64_t buffer_size;
    buffer_size   = (uint64_t) asReal(R_buffer_size); //Make sure this conv is correct ??
    INT(ret) = adios_allocate_buffer(buffer_when_value, buffer_size);
 
    return ret;
}

/**
 * To declare a ADIOS group, return adios group id pointer
 */
SEXP R_adios_declare_group(SEXP R_adios_group_name, 
                           SEXP R_adios_time_index,
                           SEXP R_adios_flag) 
{
    const char *group_name = CHARPT(R_adios_group_name, 0);
    const char *time_index = CHARPT(R_adios_time_index, 0);

    int adios_flag_value = 1111; //init dummy value
    const char *adios_flag = CHARPT(R_adios_flag, 0);
    adios_flag_value = adios_flag_hash(adios_flag); //Calling adios_flag_hash function

    SEXP R_m_adios_group;
    //Malloc
    int64_t *m_adios_group;
    m_adios_group = (int64_t *)malloc(sizeof(int64_t)); 

    //adios_declare_group (*m_adios_group, "restart", "", adios_flag_yes); // Returns group id   
    adios_declare_group(m_adios_group, group_name, time_index, adios_flag_value); // ??
    newRptr(m_adios_group, R_m_adios_group, finalizer);
    UNPROTECT(1);

    return R_m_adios_group;
}

/**
 *  To select a I/O method for a ADIOS group. Is group id a pointer here, need to check???
 */
SEXP R_adios_select_method(SEXP R_m_adios_group, 
                           SEXP R_adios_method, 
                           SEXP R_adios_params, 
                           SEXP R_adios_base_path )
{ 
    SEXP ret;
    PROTECT(ret = allocVector(INTSXP, 1));

    int64_t *group;
    group = R_ExternalPtrAddr(R_m_adios_group); // Make sure ??

    const char *method = CHARPT(R_adios_method, 0);
    const char *params = CHARPT(R_adios_params, 0);
    const char *base_path = CHARPT(R_adios_base_path, 0);

    INT(ret) = adios_select_method(*group, method, params, base_path);

    return ret;
}

/**
 * To define a ADIOS variable
 * Returns a variable ID, which can be used in adios_write_byid()
 * 0 return value indicates an error
 */
SEXP R_adios_define_var(SEXP R_m_adios_group, 
                        SEXP R_adios_varname, 
                        SEXP R_adios_path,
                        SEXP R_adios_type,
                        SEXP R_adios_local_dim, 
                        SEXP R_adios_global_dim, 
                        SEXP R_adios_local_offset)
{ 
    int adios_type_value = 1111; //init dummy value
    const char *adios_type = CHARPT(R_adios_type, 0);
    adios_type_value = adios_datatypes_hash(adios_type); //Calling adios_datatypes_hash function
    
    int64_t *group;
    group = R_ExternalPtrAddr(R_m_adios_group); // Make sure ?? 

    const char *varname = CHARPT(R_adios_varname, 0);
    const char *path = CHARPT(R_adios_path, 0);
    const char *local_dim = CHARPT(R_adios_local_dim, 0);
    const char *global_dim = CHARPT(R_adios_global_dim, 0);
    const char *local_offset = CHARPT(R_adios_local_offset, 0);

    R_debug_print("Calling adios_define_var function\n");
    R_debug_print("Value of group is %p\n",group);
    R_debug_print("The Direction is %p\n",&group);
    R_debug_print("Varname is %s\n",varname);
    R_debug_print("Path is %s\n",path);
    R_debug_print("Local dim is %s\n",local_dim);
    R_debug_print("Global dim is %s\n",global_dim);
    R_debug_print("Local_offset is %s\n",local_offset);

    SEXP R_varid;
    //Malloc
    PROTECT(R_varid = allocVector(REALSXP, 1));

    // Should "group" pass as a pointer or not ??
    REAL(R_varid)[0] = adios_define_var(*group, varname, path, adios_type_value, local_dim, global_dim, local_offset);
    UNPROTECT(1); 

    return R_varid;
}

/**
 * This function is to open or to append to an output file
 * modes = "r" = "read", "w" = "write", "a" = "append", "u" = "update"
 */
SEXP R_adios_open(SEXP R_adios_fd,
                  SEXP R_adios_group_name, 
                  SEXP R_adios_file_name, 
                  SEXP R_adios_mode, 
                  SEXP R_comm)
{ 
    SEXP ret;
    PROTECT(ret = allocVector(INTSXP, 1));

    int64_t *adios_fd = R_ExternalPtrAddr(R_m_adios_group);         

    const char *group_name = CHARPT(R_adios_group_name, 0);
    const char *file_name = CHARPT(R_adios_file_name, 0);
    const char *mode = CHARPT(R_adios_mode, 0);

    MPI_Comm comm;
    comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);

    INT(ret) = adios_open(adios_fd, group_name, file_name, mode, comm); 

    UNPROTECT(1);
    return ret;
}

/**
 * This function passes the size of the group to the internal ADIOS transport structure 
 * to facilitate the internal buffer management and to construct the group index table
 */
SEXP R_adios_group_size(SEXP R_m_adios_file, 
                        SEXP R_adios_group_size)
{
    int64_t *file_p;
    file_p = R_ExternalPtrAddr(R_m_adios_file);

    uint64_t group_size = (uint64_t) asReal(R_adios_group_size);
    R_debug_print("Group_size : %" PRIu64 "\n", group_size);
    
    SEXP R_adios_total_size;
    uint64_t *total_size;
    total_size = (uint64_t*) malloc(sizeof(uint64_t)); //Make sure this type ?? 

    R_debug_print("IN R_adios_group_size\n");
    
    // Should pass file pointer or not(Manual and example are contradict) 
    adios_group_size (*file_p, group_size, total_size); 
    newRptr(total_size, R_adios_total_size, finalizer);
    
    UNPROTECT(1);
    return R_adios_total_size; 
}

/**
 * write the data either to internal buffer or disk
 */
SEXP R_adios_write(SEXP R_m_adios_file, 
                   SEXP R_adios_var_name, 
                   SEXP R_adios_var)
{
    SEXP ret;
    PROTECT(ret = allocVector(INTSXP, 1));

    //int64_t *file_p = INTEGER(R_m_adios_file); // ??
    int64_t *file_p;
    file_p = R_ExternalPtrAddr(R_m_adios_file);

    const char *var_name; 
    var_name = CHARPT(R_adios_var_name, 0);

    void *var; 
    var = R_ExternalPtrAddr(R_adios_var); // ?? 
    INT(ret) = adios_write(*file_p, var_name, var; // Make sure ??
    
    /*int check;

    if(IS_INTEGER(R_adios_var)){
        int *int_var;
        int_var = INTEGER(R_adios_var);
        check = adios_write(*file_p, var_name, int_var);
    }else if(IS_NUMERIC(R_adios_var)){
        double *double_var;
        double_var = REAL(R_adios_var);
        check = adios_write(*file_p, var_name, double_var);
    }
    else{
        check = -1; //                                                      
    }*/
    
    R_debug_print("IN R_adios_write function call \n");

    UNPROTECT(1);
    return ret;
    
}

/**
 * commit write/read operation and close the data
 */
SEXP R_adios_close(SEXP R_m_adios_file)
{
    SEXP ret;
    PROTECT(ret = allocVector(INTSXP, 1));
    
    //int64_t *file_p = INTEGER(R_m_adios_file); // ?? 
    int64_t *file_p;
    file_p = R_ExternalPtrAddr(R_m_adios_file);

    R_debug_print("IN R_adios_close function call \n ");
    //adios_close(file_p);  // Need to pass as a pointer of not ??
    INT(ret) = adios_close(*file_p); 

    UNPROTECT(1);
    return ret;

}

/**
 * terminate ADIOS
 */
SEXP R_adios_finalize(SEXP R_comm_rank)
{
    SEXP ret;
    PROTECT(ret = allocVector(INTSXP, 1));

    R_debug_print("In R_adios_finalize\n");
    INT(ret) = adios_finalize(asInteger(R_comm_rank));
    UNPROTECT(1);
    return ret;
}
