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
 * Define variables and write data
 */
SEXP R_write(SEXP R_filename,
             SEXP R_group,
             SEXP R_groupname,
             SEXP R_nvars,          // number of vars
             SEXP R_varname_list,   // var names
             SEXP R_var_list,       // var values
             SEXP R_varlength_list, // length of var values
             SEXP R_comm,
             SEXP R_size,
             SEXP R_adios_rank)
{
    const char *filename = CHARPT(R_filename, 0); 
    int64_t m_adios_group = (int64_t)(REAL(R_group)[0]);
    const char *groupname = CHARPT(R_groupname, 0);
    int nvars = asInteger(R_nvars);
    MPI_Comm comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);
    int size = asInteger(R_size);
    int rank = asInteger(R_adios_rank);

    int i;
    int Global_bounds, Offsets; 
    uint64_t adios_groupsize, adios_totalsize;
    int64_t m_adios_file;

    // Define variables
    for(i = 0; i < nvars; i++) {
        const char *varname = CHAR(asChar(VECTOR_ELT(R_varname_list,i)));
        int length = INTEGER(R_varlength_list)[i];

        if(length == 1){
            // scalar
            adios_define_var (m_adios_group, varname, "", adios_double, 0, 0, 0);
        }else {
            // define dimensions, global_dimensions, local_offsets and the variable
            char* var1 = (char*)malloc(strlen(varname) + 20);
            strcpy(var1, varname);
            strcat(var1, "_NX");
            char* var2 = (char*)malloc(strlen(varname) + 20);
            strcpy(var2, varname);
            strcat(var2, "_Global_bounds");
            char* var3 = (char*)malloc(strlen(varname) + 20);
            strcpy(var3, varname);
            strcat(var3, "_Offsets");

            adios_define_var (m_adios_group, var1,
                              "", adios_integer, 0, 0, 0);

            adios_define_var (m_adios_group, var2,
                              "", adios_integer, 0, 0, 0);

            adios_define_var (m_adios_group, var3,
                              "", adios_integer, 0, 0, 0);

            adios_define_var (m_adios_group, varname, "", adios_double, 
                              var1, var2, var3);

            Free(var1);
            Free(var2);
            Free(var3);
        }
    }

    // Open ADIOS
    adios_open (&m_adios_file, groupname, filename, "w", comm);

    adios_groupsize = 0;
    for(i = 0; i < nvars; i++) {
        int length = INTEGER(R_varlength_list)[i];
        if(length == 1) {
            adios_groupsize += 8;
        }else {
            adios_groupsize += (4 + 4 + 4 + length * 8);
        }
    }

    adios_group_size (m_adios_file, adios_groupsize, &adios_totalsize);

    // Write data into variables
    for(i = 0; i < nvars; i++) {
        const char *varname = CHAR(asChar(VECTOR_ELT(R_varname_list,i)));
        int length = INTEGER(R_varlength_list)[i];
        double *data = REAL(VECTOR_ELT(R_var_list, i));

        if(length == 1){
            // scalar
            adios_write(m_adios_file, varname, (void *) data);
        }else {
            // var
            char* var1 = (char*)malloc(strlen(varname) + 20);
            strcpy(var1, varname);
            strcat(var1, "_NX");
            char* var2 = (char*)malloc(strlen(varname) + 20);
            strcpy(var2, varname);
            strcat(var2, "_Global_bounds");
            char* var3 = (char*)malloc(strlen(varname) + 20);
            strcpy(var3, varname);
            strcat(var3, "_Offsets");

            adios_write(m_adios_file, var1, (void *) &length);

            Global_bounds = length * size;
            adios_write(m_adios_file, var2, (void *) &Global_bounds);

            Offsets = rank * length;
            adios_write(m_adios_file, var3, (void *) &Offsets);

            adios_write(m_adios_file, varname, data);
        }
       
    }

    adios_close (m_adios_file);
    MPI_Barrier (comm);

    adios_finalize (rank);
}

