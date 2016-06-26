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
                      SEXP R_adios_attrname,
                      SEXP R_adios_getattr)
{
    ADIOS_FILE *fp = R_ExternalPtrAddr(R_adios_fp);
    const char *attrname = CHARPT(R_adios_attrname, 0);
    struct ATTR *getattr = R_ExternalPtrAddr(R_adios_getattr);

    adios_get_attr(fp, 
                   attrname, 
                   &getattr->type, 
                   &getattr->size, 
                   &getattr->data);

    return R_NilValue;
}

/**
 *  Convenience function to get an attribute by name
 *       attrid   index of attribute (0..fp->nattrs-1)
 *                in fp->attr_namelist of ADIOS_FILE struct
 */
SEXP R_adios_get_attr_byid(SEXP R_adios_fp, 
                           SEXP R_adios_attrid)
{
    ADIOS_FILE *fp = R_ExternalPtrAddr(R_adios_fp);
    int attrid = asInteger(R_adios_attrid);

    /*struct ATTR *getattr;
    SEXP R_getattr;

    adios_get_attr_byid(fp, 
                        attrid, 
                        &getattr->type, 
                        &getattr->size, 
                        &getattr->data);

    newRptr(getattr, R_getattr, finalizer);
    UNPROTECT(1);

    return R_getattr;*/
}

/**
 * print the adios attribute
 */
SEXP R_adios_print_attr(SEXP R_adios_rank,
                        SEXP R_adios_fp,
                        SEXP R_adios_attrid,
                        SEXP R_adios_get_attr)
{
    int rank = asInteger(R_adios_rank);
    ADIOS_FILE * fp = R_ExternalPtrAddr(R_adios_fp);
    int attrid = asInteger(R_adios_attrid);

    struct ATTR *getattr = R_ExternalPtrAddr(R_adios_get_attr);
    enum ADIOS_DATATYPES attr_type = getattr->type;
    int attr_size = getattr->size;
    void *data = getattr->data;

    // store attribute information into vector

    Rprintf ("rank %d: attr: %s %s = ", rank, adios_type_to_string(attr_type), fp->attr_namelist[attrid]);
    int type_size = adios_type_size(attr_type, data);
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
    return R_NilValue;
}

/**
 * print all the adios attributes
 */
SEXP R_adios_attr_read(SEXP R_adios_rank,
                        SEXP R_adios_fp)
{   
    int i;
    ADIOS_FILE * fp = R_ExternalPtrAddr(R_adios_fp);
    int rank = asInteger(R_adios_rank);

    struct ATTR attrs;
    SEXP R_adios_getattr;
    newRptr(&attrs, R_adios_getattr, finalizer);
    

    for (i = 0; i < fp->nattrs; i++)
    {       
        R_adios_get_attr(R_adios_fp, mkString(fp->attr_namelist[i]), R_adios_getattr);
        R_adios_print_attr(R_adios_rank, R_adios_fp, ScalarInteger(i), R_adios_getattr);
        Free(attrs.data);
        attrs.data = 0;
    }

    UNPROTECT(1);
    return R_NilValue;
}

