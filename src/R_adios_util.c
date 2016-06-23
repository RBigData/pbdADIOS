typedef struct{
    enum ADIOS_DATATYPES *type;
    int *size;
    void **data;
}Composite;

/**
 *  Get an attribute in a file.
 *  This function does not read anything from the file but processes info
 *  already in memory after fopen.
 *  The memory for the data is allocated within the library.
 *  You can use free() to free the memory after use.
 *  Return: pointer to the attr value.
 */
SEXP R_adios_get_attr(SEXP R_adios_fp, 
                      SEXP R_adios_attrname, 
                      SEXP R_adios_type,
                      SEXP R_adios_size,
                      SEXP R_adios_data)
{
    ADIOS_FILE *fp;
    fp = R_ExternalPtrAddr(R_adios_fp);
    const char *attrname = CHARPT(R_adios_attrname, 0);
    // R_adios_type is integer here.
    enum ADIOS_DATATYPES type = asInteger(R_adios_type);


    adios_get_attr(fp, 
                   attrname, 
                   type, 
                   size, 
                   data);
}

/**
 *  Get error number
 */
SEXP R_adios_get_attr_byid(SEXP R_adios_fp, 
                           SEXP R_adios_attrid, 
                           SEXP R_adios_type,
                           SEXP R_adios_size,
                           SEXP R_adios_data,)
{
    
}
int adios_get_attr_byid (ADIOS_FILE  * fp, int attrid, 
                         enum ADIOS_DATATYPES * type,
                         int * size, void ** data);