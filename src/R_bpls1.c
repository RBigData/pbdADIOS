#include <inttypes.h>
#include <stdbool.h>
#include "R_adios.h"

void mergeLists(int nV, char **listV, int nA, char **listA, char **mlist, bool *isVar);
int print_data(void *data, int item, enum ADIOS_DATATYPES adiosvartype);
int doList_group (ADIOS_FILE *fp);


/**
 * R wrapper of bpls
 */
SEXP R_bpls(SEXP R_adios_path,
            SEXP R_comm,
            SEXP R_adios_rank)
//int doList(const char *path) 
{
    ADIOS_FILE  *fp;
    int status;
    const char *path = CHARPT(R_adios_path, 0);
    MPI_Comm comm;
    comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);
    int rank = asInteger(R_adios_rank);

    /*if (!rank) {
        
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

        doList_group (fp);
        
        adios_read_close (fp);
        adios_read_finalize_method(ADIOS_READ_METHOD_BP);
    }*/

    MPI_Comm_rank (comm, &rank);

    if(!rank) {
        MPI_Group group_world;
        MPI_Group new_group;
        MPI_Comm new_comm;

        int process_rank[] = {0};

        //get the group under MPI_COMM_WORLD
        MPI_Comm_group(comm, &group_world);
        // create the new group
        MPI_Group_incl(group_world, 1, process_rank, &new_group);
        // create the new communicator
        MPI_Comm_create(comm, new_group, &new_comm);

        status = adios_read_init_method (ADIOS_READ_METHOD_BP, new_comm, "verbose=2");
        if (status) {
            REprintf("Error: %s\n", adios_errmsg());
            exit(6);
        }

        // open the BP file
        fp = adios_read_open_file (path, ADIOS_READ_METHOD_BP, new_comm); 
        if (fp == NULL) {
            exit(7);
        }

        doList_group (fp);
        
        adios_read_close (fp);
        adios_read_finalize_method(ADIOS_READ_METHOD_BP);

        MPI_Group_free(&group_world);
        MPI_Group_free(&new_group);
        MPI_Comm_free(&new_comm);
    }

    /*status = adios_read_init_method (ADIOS_READ_METHOD_BP, comm, "verbose=2");
    if (status) {
        REprintf("Error: %s\n", adios_errmsg());
        exit(6);
    }

    // open the BP file
    fp = adios_read_open_file (path, ADIOS_READ_METHOD_BP, comm); 
    if (fp == NULL) {
        exit(7);
    }

    doList_group (fp);
    
    adios_read_close (fp);
    adios_read_finalize_method(ADIOS_READ_METHOD_BP);*/

    return R_NilValue;
}

/** 
 * merge vars list and attrs list
 */
void mergeLists(int nV, char **listV, int nA, char **listA, char **mlist, bool *isVar) 
{
    int v, a, idx;

    // first add vars then attrs (if ask ed)
    idx = 0;
        
    for (v=0; v<nV; v++) {
        mlist[idx] = listV[v];
        isVar[idx] = true;
        idx++;
    }

    for (a=0; a<nA; a++) {
        mlist[idx] = listA[a];
        isVar[idx] = false;
        idx++;
    }
        
}

/**
 * print data
 */
int print_data(void *data, int item, enum ADIOS_DATATYPES adiosvartype)
{
    if (data == NULL) {
        Rprintf("null ");
        return 0;
    }
    // print next data item 
    switch(adiosvartype) {
        case adios_unsigned_byte:
            Rprintf("%hhu", ((unsigned char *) data)[item]);
            break;
        case adios_byte:
            Rprintf("%hhd", ((signed char *) data)[item]);
            break;

        case adios_string:
            Rprintf("\"%s\"", ((char *) data)+item);
            break;
        case adios_string_array:
            // we expect one elemet of the array here
            Rprintf("\"%s\"", *((char **)data+item));
            break;

        case adios_unsigned_short:  
            Rprintf("%hu", ((unsigned short *) data)[item]);
            break;
        case adios_short:
            Rprintf("%hd", ((signed short *) data)[item]);
            break;

        case adios_unsigned_integer:
            Rprintf("%u", ((unsigned int *) data)[item]);
            break;
        case adios_integer:    
            Rprintf("%d", ((signed int *) data)[item]);
            break;

        case adios_unsigned_long:
            Rprintf("%llu", ((unsigned long long *) data)[item]);
            break;
        case adios_long:        
            Rprintf("%lld", ((signed long long *) data)[item]);
            break;

        case adios_real:
            Rprintf("%g", ((float *) data)[item]);
            break;

        case adios_double:
            Rprintf("%g", ((double *) data)[item]);
            break;

        case adios_long_double:
            Rprintf("%Lg", ((long double *) data)[item]);
            //Rprintf(outf,(f ? format : "????????"));
            break;

        case adios_complex:  
            Rprintf("(%g,i%g)", ((float *) data)[2*item], ((float *) data)[2*item+1]);
            break;

        case adios_double_complex:
            Rprintf("(%g,i%g)", ((double *) data)[2*item], ((double *) data)[2*item+1]);
            break;

        default:
            break;
    } // end switch
    return 0;
}


int doList_group (ADIOS_FILE *fp)
{
    bool readattrs = true;
    bool dump = false;
    bool timestep = false;
    char commentchar;
    commentchar = ' ';
    ADIOS_VARINFO *vi; 
    ADIOS_VARINFO **vis; 
    enum ADIOS_DATATYPES vartype;
    int     i, j, n;             // loop vars
    int     attrsize;                       // info about one attribute
    bool    matches;
    int     len, maxlen, maxtypelen;
    int     retval;
    char  **names;  // vars and attrs together, sorted or unsorted
    bool   *isVar;  // true for each var, false for each attr
    int     nNames; // number of vars + attrs
    void   *value;  // scalar value is returned by get_attr
    bool    timed;  // variable has multiple timesteps

    nNames = fp->nvars + fp->nattrs;

    names = (char **) malloc (nNames * sizeof (char*)); // store only pointers
    isVar = (bool *) malloc (nNames * sizeof (bool));
    vis   = (ADIOS_VARINFO **) malloc (nNames * sizeof (ADIOS_VARINFO*));
    if (names == NULL || isVar == NULL || vis == NULL) {
        REprintf("Error: could not allocate char* and bool arrays of %d elements\n", nNames);
        return 5;
    }
    mergeLists(fp->nvars, fp->var_namelist, fp->nattrs, fp->attr_namelist, names, isVar);

    // calculate max length of variable names in the first round
    maxlen = 4;
    for (n=0; n<nNames; n++) {
        len = strlen(names[n]);
        if (len > maxlen) maxlen = len;
    }

    // Get VARINFO's and attr types and calculate max length of type names 
    maxtypelen = 7;
    for (n=0; n<nNames; n++) {
        if (isVar[n])  {
            vis[n] = adios_inq_var (fp, names[n]);
            if (!vis[n]) {
                REprintf("Error: %s\n", adios_errmsg());
            }
            vartype = vis[n]->type;
        } else {
            retval = adios_get_attr (fp, names[n], &vartype, &attrsize, &value);
            if (retval) {
                REprintf("Error: %s\n", adios_errmsg());
            }
        }
        len = strlen(adios_type_to_string(vartype));
        if (len > maxtypelen) maxtypelen = len;
    }

    /* VARIABLES */
    for (n=0; n<nNames; n++) {
        matches = true;
        if (isVar[n])  {
            vi = vis[n];
            vartype = vi->type;
            //timed = adios_read_bp_is_var_timed(fp, vi->varid);
            timed = (vi->nsteps > 1);
        } else {
            retval = adios_get_attr (fp, names[n], &vartype, &attrsize, &value);
            if (retval) {
                REprintf("Error: %s\n", adios_errmsg());
            }
        }

        if (matches) {
            // print definition of variable
            Rprintf("%c %-*s  %-*s", commentchar, maxtypelen, 
                    adios_type_to_string(vartype), maxlen, names[n]); 
            if (!isVar[n]) {
                // list (and print) attribute
                if (readattrs || dump) {
                    Rprintf("  attr   = ");
                    int type_size = adios_type_size (vartype, value);
                    int nelems = attrsize / type_size;
                    char *p = (char*)value;
                    if (nelems>1) Rprintf("{");
                    for (i=0; i<nelems; i++) { 
                        if (i>0) Rprintf(", ");
                        print_data(p, 0, vartype); 
                        p += type_size;
                    }
                    if (nelems>1) Rprintf("}");
                    Rprintf("\n");
                    matches = false; // already printed
                } else {
                    Rprintf("  attr\n");
                }
            } else if (!vi) { 
                // after error
                Rprintf("\n");
            } else if (vi->ndim > 0 || timed) {

                Rprintf("  ");
                if (timed) 
                    Rprintf("%d*", vi->nsteps);
                if (vi->ndim > 0) {
                    Rprintf("{%" PRId64, vi->dims[0]);
                    for (j=1; j < vi->ndim; j++) {
                        Rprintf(", %" PRId64, vi->dims[j]);
                    }
                    Rprintf("}");
                } else {
                    Rprintf("scalar");
                }

                if (vi->statistics) {

                    if(timestep == false || timed == false) {

                        Rprintf(" = ");
                        if(vartype == adios_complex || vartype == adios_double_complex) {
                            // force printing (double,double) here
                            print_data(vi->statistics->min, 0, adios_double_complex); 
                            Rprintf(" / ");
                            print_data(vi->statistics->max, 0, adios_double_complex); 
                            Rprintf(" / ");
                            print_data(vi->statistics->avg, 0, adios_double_complex);
                            Rprintf(" / ");
                            print_data(vi->statistics->std_dev, 0, adios_double_complex);
                        } else {
                            print_data(vi->statistics->min, 0, vartype); 
                            Rprintf(" / ");
                            print_data(vi->statistics->max, 0, vartype); 
                            Rprintf(" / ");
                            print_data(vi->statistics->avg, 0, adios_double);
                            Rprintf(" / ");
                            print_data(vi->statistics->std_dev, 0, adios_double);
                        }

                        //Rprintf(outf," {MIN / MAX / AVG / STD_DEV} ");
                    } 
                } // longopt && vi->statistics 
                Rprintf("\n");

            } else {
                // scalar
                Rprintf("  scalar");
                if (vi->value) {
                    Rprintf(" = ");
                    print_data(vi->value, 0, vartype); 
                    matches = false; // already printed
                }
                Rprintf("\n");
            }
        }

        if (!isVar[n])
            free(value);
    }
    /* Free ADIOS_VARINFOs */
    for (n=0; n<nNames; n++) {
        if (isVar[n])  {
            adios_free_varinfo(vis[n]);
        }
    }
    free(names);
    free(isVar);
    return 0;
} 