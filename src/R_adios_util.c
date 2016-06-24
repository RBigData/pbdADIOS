#include "R_adios.h"

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
        Free(ptr->data);
        Free(ptr);
        R_debug_print("finalizer: Freed %p.\n", ptr);
        R_ClearExternalPtr(Rptr);
        R_debug_print("finalizer: %p Cleared Rptr.\n", ptr);
    }
}

/**
 *  Get an attribute in a file.
 *  This function does not read anything from the file but processes info
 *  already in memory after fopen.
 *  The memory for the data is allocated within the library.
 *  You can use free() to free the memory after use.
 *  Return: struct of type, size and data.
 */
SEXP R_adios_get_attr(SEXP R_adios_fp, 
                      SEXP R_adios_attrname)
{
    ADIOS_FILE *fp;
    fp = R_ExternalPtrAddr(R_adios_fp);
    const char *attrname = CHARPT(R_adios_attrname, 0);

    struct ATTR *getattr;
    SEXP R_getattr;

    adios_get_attr(fp, 
                   attrname, 
                   &getattr->type, 
                   &getattr->size, 
                   &getattr->data);

    newRptr(getattr, R_getattr, finalizer);
    UNPROTECT(1);

    return R_getattr;
}

/**
 *  Convenience function to get an attribute by name
 *       attrid   index of attribute (0..fp->nattrs-1)
 *                in fp->attr_namelist of ADIOS_FILE struct
 */
SEXP R_adios_get_attr_byid(SEXP R_adios_fp, 
                           SEXP R_adios_attrid)
{
    ADIOS_FILE *fp;
    fp = R_ExternalPtrAddr(R_adios_fp);
    int attrid = asInteger(R_adios_attrid);

    struct ATTR *getattr;
    SEXP R_getattr;

    adios_get_attr(fp, 
                   attrid, 
                   &getattr->type, 
                   &getattr->size, 
                   &getattr->data);

    newRptr(getattr, R_getattr, finalizer);
    UNPROTECT(1);

    return R_getattr;
}

/**
 * print the adios attribute
 */
SEXP R_adios_print_attr(SEXP R_adios_rank,
                        SEXP R_adios_fp,
                        SEXP R_adios_attrid,
                        SEXP R_adios_type,
                        SEXP R_adios_size,
                        SEXP R_adios_data)
{
    int rank = asInteger(R_adios_rank);
    ADIOS_FILE * fp = R_ExternalPtrAddr(R_adios_file_ptr);
    int attrid = asInteger(R_adios_attrid);
    enum ADIOS_DATATYPES attr_type = asInteger(R_adios_type);
    int attr_size = asInteger(R_adios_size);
    void *data = R_ExternalPtrAddr(R_adios_data);

    Rprintf ("rank %d: attr: %s %s = ", rank, adios_type_to_string(attr_type), f->attr_namelist [attrid]);
    int type_size = adios_type_size (attr_type, data);
    int nelems = attr_size / type_size;
    int k;
    char *p = (char*)data;
    for (k=0; k<nelems; k++) 
    {
        if (k>0) printf(", ");
        switch (attr_type)  
        {
            case adios_integer:
                Rprintf ("%d", *(int *)p);
                break;
            case adios_double:
                Rprintf ("%e", *(double *)p);
                break;
            case adios_string:
                Rprintf ("\"%s\"", (char *)p);
                break;
            case adios_string_array:
                Rprintf ("\"%s\"", *(char **)p);
                break;
            default:
                Rprintf ("??????\n");
        }
        p=p+type_size;
    }
    Rprintf("\n");
}

/**
 * print the adios attributes
 */
SEXP R_adios_attr_read(SEXP R_adios_rank,
                        SEXP R_adios_fp)
{   
    int i;

    for (i = 0; i < fp->nattrs; i++)
    {
        ADIOS_FILE * fp = R_ExternalPtrAddr(R_adios_file_ptr);
        SEXP R_getattr;
       
        R_getattr = R_adios_get_attr(R_adios_fp, mkString(fp->attr_namelist[i]));
        R_adios_print_attr(R_adios_rank, R_adios_fp, ScalarInteger(i), R_getattr);
    }
}

