#include "R_bpls.h"
#include "R_dump.h"
#include "R_write.h"

/**
 *  Higher level implementation of ADIOS write API
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
 * ADIOS init and create group etc.
 * Return: pointer to the ADIOS group structure
 */
SEXP R_create(SEXP R_groupname,
              SEXP R_buffersize,
              SEXP R_comm)
{
    const char *groupname = CHARPT(R_groupname, 0);
    int buffer = asInteger(R_buffersize);
    MPI_Comm comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);

    int64_t m_adios_group;
    
    adios_init_noxml (comm);
    adios_set_max_buffer_size (buffer); // Default buffer size for write is 20. User can change this value

    adios_declare_group (&m_adios_group, groupname, "", adios_flag_yes);
    adios_select_method (m_adios_group, "MPI", "", ""); // Default method is MPI. Let users choose different methods later.

    // Pass group pointer to R
    SEXP R_group = PROTECT(allocVector(REALSXP, 1));
    REAL(R_group)[0] = (double)m_adios_group;
    UNPROTECT(1);

    return R_group;
}

/**
 * Define attributes in the bp file
 */
SEXP R_define_attr(SEXP R_group,
                   SEXP R_attrname,
                   SEXP R_nelems,
                   SEXP R_values)
{
    int64_t m_adios_group = (int64_t)(REAL(R_group)[0]);
    const char *attrname = CHARPT(R_attrname, 0); 
    int nelems = asInteger(R_nelems);
    char **values;
    values = malloc(nelems);

    int i;
    for(i = 0; i < nelems; i++)
        values[i] = (char*)CHAR(STRING_ELT(R_values,i));

    adios_define_attribute_byvalue(m_adios_group,
                                   attrname, "",
                                   adios_string_array,
                                   nelems,
                                   values);

    return R_NilValue;
}

/**
 * Define variables and write data
 */
SEXP R_write(SEXP R_filename,
             SEXP R_group,
             SEXP R_groupname,
             SEXP R_nvars,          // number of vars
             SEXP R_varname_list,   // var names
             SEXP R_var_list,       // var values
             SEXP R_varlength_list, // length of var values
             SEXP R_ndim,           // number of dims
             SEXP R_type, 
             SEXP R_comm,
             SEXP R_p,
             SEXP R_adios_rank)
{
    const char *filename = CHARPT(R_filename, 0); 
    int64_t m_adios_group = (int64_t)(REAL(R_group)[0]);
    const char *groupname = CHARPT(R_groupname, 0);
    int nvars = asInteger(R_nvars);
    MPI_Comm comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);
    int size = asInteger(R_p);
    int rank = asInteger(R_adios_rank);
    
    int i, j;
    int Global_bounds, Offsets; 
    uint64_t adios_groupsize, adios_totalsize;
    int64_t m_adios_file;

    // variable to store the value converted from integer
    char str[256];

    // Define variables
    for(i = 0; i < nvars; i++) {
        const char *varname = CHAR(asChar(VECTOR_ELT(R_varname_list,i)));
        int *length = INTEGER(VECTOR_ELT(R_varlength_list, i));
        int *vndim = INTEGER(VECTOR_ELT(R_ndim, i));
        int *typetag = INTEGER(VECTOR_ELT(R_type, i));

        if((length[0] == 1) && (vndim[0] == 1)){
            // scalar
            if(typetag[0] == 0) {
                adios_define_var (m_adios_group, varname, "", adios_integer, 0, 0, 0);
            }else {
                adios_define_var (m_adios_group, varname, "", adios_double, 0, 0, 0);
            }
        }else {
            // define dimensions, global_dimensions, local_offsets and the variable
            int temp_var_length = strlen(varname) + 8;
            char* local_var = (char*)malloc(vndim[0]*temp_var_length);
            char* global_var = (char*)malloc(vndim[0]*temp_var_length);
            char* offset_var = (char*)malloc(vndim[0]*temp_var_length);

            // initialize char variables
            strcpy(local_var, "");
            strcpy(global_var, "");
            strcpy(offset_var, "");

            // j = 0
            j = 0;
            sprintf(str, "%d", j);

            char* local = (char*)malloc(temp_var_length);
            strcpy(local, varname);
            strcat(local, "_nx_");
            strcat(local, str);
            strcat(local_var, local);

            char* global = (char*)malloc(temp_var_length);
            strcpy(global, varname);
            strcat(global, "_gx_");
            strcat(global, str);
            strcat(global_var, global);

            char* offset = (char*)malloc(temp_var_length);
            strcpy(offset, varname);
            strcat(offset, "_off_");
            strcat(offset, str);
            strcat(offset_var, offset);

            // define local dim, global dim and offset for each dimension
            adios_define_var (m_adios_group, local,
                          "", adios_integer, 0, 0, 0);
            adios_define_var (m_adios_group, global,
                          "", adios_integer, 0, 0, 0);
            adios_define_var (m_adios_group, offset,
                          "", adios_integer, 0, 0, 0);

            Free(local);
            Free(global);
            Free(offset);

            for(j = 1; j < vndim[0]; j++) {
                sprintf(str, "%d", j);

                strcat(local_var, ",");
                char* local = (char*)malloc(temp_var_length);
                strcpy(local, varname);
                strcat(local, "_nx_");
                strcat(local, str);
                strcat(local_var, local);

                strcat(global_var, ",");
                char* global = (char*)malloc(temp_var_length);
                strcpy(global, varname);
                strcat(global, "_gx_");
                strcat(global, str);
                strcat(global_var, global);

                strcat(offset_var, ",");
                char* offset = (char*)malloc(temp_var_length);
                strcpy(offset, varname);
                strcat(offset, "_off_");
                strcat(offset, str);
                strcat(offset_var, offset);

                // define local dim, global dim and offset for each dimension
                adios_define_var (m_adios_group, local,
                              "", adios_integer, 0, 0, 0);
                adios_define_var (m_adios_group, global,
                              "", adios_integer, 0, 0, 0);
                adios_define_var (m_adios_group, offset,
                              "", adios_integer, 0, 0, 0);

                Free(local);
                Free(global);
                Free(offset);
            }

            // define variable
            if(typetag[0] == 0) {
                adios_define_var (m_adios_group, varname, "", adios_integer, 
                              local_var, global_var, offset_var);
            }else {
                adios_define_var (m_adios_group, varname, "", adios_double, 
                              local_var, global_var, offset_var);
            }

            Free(local_var);
            Free(global_var);
            Free(offset_var);
        }
    }

    // Open ADIOS
    adios_open (&m_adios_file, groupname, filename, "w", comm);

    adios_groupsize = 0;
    for(i = 0; i < nvars; i++) {
        int *length = INTEGER(VECTOR_ELT(R_varlength_list, i));
        int *vndim = INTEGER(VECTOR_ELT(R_ndim, i));
        int *typetag = INTEGER(VECTOR_ELT(R_type, i));

        // calculate the length of the variable
        int temp = 1;
        for(j = 0; j < vndim[0]; j++)
            temp *= length[j];

        if((length[0] == 1) && (vndim[0] == 1)){
            // scalar
            if(typetag[0] == 0) {
                adios_groupsize += 4;
            }else {
                adios_groupsize += 8;
            }
        }else {
            if(typetag[0] == 0) {
                adios_groupsize += (12 * vndim[0] + temp * 4);
            }else {
                adios_groupsize += (12 * vndim[0] + temp * 8);
            }
        }
    }

    adios_group_size (m_adios_file, adios_groupsize, &adios_totalsize);

    // Write data into variables
    for(i = 0; i < nvars; i++) {
        const char *varname = CHAR(asChar(VECTOR_ELT(R_varname_list,i)));
        int *length = INTEGER(VECTOR_ELT(R_varlength_list, i));
        int *vndim = INTEGER(VECTOR_ELT(R_ndim, i));
        int *typetag = INTEGER(VECTOR_ELT(R_type, i));

        if((length[0] == 1) && (vndim[0] == 1)){
            // scalar
        }else {
            // var
            int temp_var_length = strlen(varname) + 8;

            j = 0;
            sprintf(str, "%d", j);

            char* local = (char*)malloc(temp_var_length);
            strcpy(local, varname);
            strcat(local, "_nx_");
            strcat(local, str);

            char* global = (char*)malloc(temp_var_length);
            strcpy(global, varname);
            strcat(global, "_gx_");
            strcat(global, str);

            char* offset = (char*)malloc(temp_var_length);
            strcpy(offset, varname);
            strcat(offset, "_off_");
            strcat(offset, str);

            adios_write(m_adios_file, local, (void *) &(length[j]));

            Global_bounds = length[j] * size;
            adios_write(m_adios_file, global, (void *) &Global_bounds);

            Offsets = rank * length[j];
            adios_write(m_adios_file, offset, (void *) &Offsets);

            Free(local);
            Free(global);
            Free(offset);
            for(j = 1; j < vndim[0]; j++) {
                sprintf(str, "%d", j);

                char* local = (char*)malloc(temp_var_length);
                strcpy(local, varname);
                strcat(local, "_nx_");
                strcat(local, str);

                char* global = (char*)malloc(temp_var_length);
                strcpy(global, varname);
                strcat(global, "_gx_");
                strcat(global, str);

                char* offset = (char*)malloc(temp_var_length);
                strcpy(offset, varname);
                strcat(offset, "_off_");
                strcat(offset, str);

                adios_write(m_adios_file, local, (void *) &(length[j]));

                Global_bounds = length[j];
                adios_write(m_adios_file, global, (void *) &Global_bounds);

                Offsets = 0;
                adios_write(m_adios_file, offset, (void *) &Offsets);

                Free(local);
                Free(global);
                Free(offset);
            }
        }
        // write var data
        if(typetag[0] == 0) {
            adios_write(m_adios_file, varname, (void *) INTEGER(VECTOR_ELT(R_var_list, i)));
        }else {
            adios_write(m_adios_file, varname, (void *) REAL(VECTOR_ELT(R_var_list, i)));
        }
    }
    adios_close (m_adios_file);
    MPI_Barrier (comm);

    return R_NilValue;
}

/**
 * Append data
 */
SEXP R_append(SEXP R_filename,
              SEXP R_group,
              SEXP R_groupname,
              SEXP R_nvars,          // number of vars
              SEXP R_varname_list,   // var names
              SEXP R_var_list,       // var values
              SEXP R_varlength_list, // length of var values
              SEXP R_ndim,           // number of dims
              SEXP R_type, 
              SEXP R_comm,
              SEXP R_p,
              SEXP R_adios_rank)
{
    const char *filename = CHARPT(R_filename, 0); 
    int64_t m_adios_group = (int64_t)(REAL(R_group)[0]);
    const char *groupname = CHARPT(R_groupname, 0);
    int nvars = asInteger(R_nvars);
    MPI_Comm comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);
    int size = asInteger(R_p);
    int rank = asInteger(R_adios_rank);

    int i, j;
    int Global_bounds, Offsets; 
    uint64_t adios_groupsize, adios_totalsize;
    int64_t m_adios_file;

    // variable to store the value converted from integer
    char str[256];

    // Open ADIOS and append data
    adios_open (&m_adios_file, groupname, filename, "a", comm);

    adios_groupsize = 0;
    for(i = 0; i < nvars; i++) {
        int *length = INTEGER(VECTOR_ELT(R_varlength_list, i));
        int *vndim = INTEGER(VECTOR_ELT(R_ndim, i));
        int *typetag = INTEGER(VECTOR_ELT(R_type, i));

        // calculate the length of the variable
        int temp = 1;
        for(j = 0; j < vndim[0]; j++)
            temp *= length[j];

        if((length[0] == 1) && (vndim[0] == 1)){
            // scalar
            if(typetag[0] == 0) {
                adios_groupsize += 4;
            }else {
                adios_groupsize += 8;
            }
        }else {
            if(typetag[0] == 0) {
                adios_groupsize += (12 * vndim[0] + temp * 4);
            }else {
                adios_groupsize += (12 * vndim[0] + temp * 8);
            }
        }
    }

    adios_group_size (m_adios_file, adios_groupsize, &adios_totalsize);

    // Write data into variables
    for(i = 0; i < nvars; i++) {
        const char *varname = CHAR(asChar(VECTOR_ELT(R_varname_list,i)));
        int *length = INTEGER(VECTOR_ELT(R_varlength_list, i));
        int *vndim = INTEGER(VECTOR_ELT(R_ndim, i));
        int *typetag = INTEGER(VECTOR_ELT(R_type, i));
        
        if((length[0] == 1) && (vndim[0] == 1)){
            // scalar
        }else {
            // var
            int temp_var_length = strlen(varname) + 8;

            j = 0;
            sprintf(str, "%d", j);

            char* local = (char*)malloc(temp_var_length);
            strcpy(local, varname);
            strcat(local, "_nx_");
            strcat(local, str);

            char* global = (char*)malloc(temp_var_length);
            strcpy(global, varname);
            strcat(global, "_gx_");
            strcat(global, str);

            char* offset = (char*)malloc(temp_var_length);
            strcpy(offset, varname);
            strcat(offset, "_off_");
            strcat(offset, str);

            adios_write(m_adios_file, local, (void *) &(length[j]));

            Global_bounds = length[j] * size;
            adios_write(m_adios_file, global, (void *) &Global_bounds);

            Offsets = rank * length[j];
            adios_write(m_adios_file, offset, (void *) &Offsets);

            Free(local);
            Free(global);
            Free(offset);
            for(j = 1; j < vndim[0]; j++) {
                sprintf(str, "%d", j);

                char* local = (char*)malloc(temp_var_length);
                strcpy(local, varname);
                strcat(local, "_nx_");
                strcat(local, str);

                char* global = (char*)malloc(temp_var_length);
                strcpy(global, varname);
                strcat(global, "_gx_");
                strcat(global, str);

                char* offset = (char*)malloc(temp_var_length);
                strcpy(offset, varname);
                strcat(offset, "_off_");
                strcat(offset, str);

                adios_write(m_adios_file, local, (void *) &(length[j]));

                Global_bounds = length[j];
                adios_write(m_adios_file, global, (void *) &Global_bounds);

                Offsets = 0;
                adios_write(m_adios_file, offset, (void *) &Offsets);

                Free(local);
                Free(global);
                Free(offset);
            }
        }
        // write var data
        if(typetag[0] == 0) {
            adios_write(m_adios_file, varname, (void *) INTEGER(VECTOR_ELT(R_var_list, i)));
        }else {
            adios_write(m_adios_file, varname, (void *) REAL(VECTOR_ELT(R_var_list, i)));
        }
    }
    adios_close (m_adios_file);
    MPI_Barrier (comm);

    return R_NilValue;
}

