#include <inttypes.h>
#include "R_adios.h"

FILE *outf;   // file to print to or stdout
char commentchar;
commentchar = ' ';

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

int print_data(void *data, int item, enum ADIOS_DATATYPES adiosvartype, bool allowformat)
{
    bool f = formatgiven && allowformat;
    if (data == NULL) {
        fprintf(outf, "null ");
        return 0;
    }
    // print next data item 
    switch(adiosvartype) {
        case adios_unsigned_byte:
            fprintf(outf,(f ? format : "%hhu"), ((unsigned char *) data)[item]);
            break;
        case adios_byte:
            fprintf(outf,(f ? format : "%hhd"), ((signed char *) data)[item]);
            break;

        case adios_string:
            fprintf(outf,(f ? format : "\"%s\""), ((char *) data)+item);
            break;
        case adios_string_array:
            // we expect one elemet of the array here
            fprintf(outf,(f ? format : "\"%s\""), *((char **)data+item));
            break;

        case adios_unsigned_short:  
            fprintf(outf,(f ? format : "%hu"), ((unsigned short *) data)[item]);
            break;
        case adios_short:
            fprintf(outf,(f ? format : "%hd"), ((signed short *) data)[item]);
            break;

        case adios_unsigned_integer:
            fprintf(outf,(f ? format : "%u"), ((unsigned int *) data)[item]);
            break;
        case adios_integer:    
            fprintf(outf,(f ? format : "%d"), ((signed int *) data)[item]);
            break;

        case adios_unsigned_long:
            fprintf(outf,(f ? format : "%llu"), ((unsigned long long *) data)[item]);
            break;
        case adios_long:        
            fprintf(outf,(f ? format : "%lld"), ((signed long long *) data)[item]);
            break;

        case adios_real:
            fprintf(outf,(f ? format : "%g"), ((float *) data)[item]);
            break;

        case adios_double:
            fprintf(outf,(f ? format : "%g"), ((double *) data)[item]);
            break;

        case adios_long_double:
            fprintf(outf,(f ? format : "%Lg"), ((long double *) data)[item]);
            //fprintf(outf,(f ? format : "????????"));
            break;

        case adios_complex:  
            fprintf(outf,(f ? format : "(%g,i%g)"), ((float *) data)[2*item], ((float *) data)[2*item+1]);
            break;

        case adios_double_complex:
            fprintf(outf,(f ? format : "(%g,i%g)" ), ((double *) data)[2*item], ((double *) data)[2*item+1]);
            break;

        default:
            break;
    } // end switch
    return 0;
}


int doList_group (ADIOS_FILE *fp)
{
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
        fprintf(stderr, "Error: could not allocate char* and bool arrays of %d elements\n", nNames);
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
                fprintf(stderr, "Error: %s\n", adios_errmsg());
            }
            vartype = vis[n]->type;
        } else {
            retval = adios_get_attr (fp, names[n], &vartype, &attrsize, &value);
            if (retval) {
                fprintf(stderr, "Error: %s\n", adios_errmsg());
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
                fprintf(stderr, "Error: %s\n", adios_errmsg());
            }
        }

        if (matches) {

            // print definition of variable
            fprintf(outf,"%c %-*s  %-*s", commentchar, maxtypelen, 
                    adios_type_to_string(vartype), maxlen, names[n]); 
            if (!isVar[n]) {
                // list (and print) attribute
                if (readattrs || dump) {
                    fprintf(outf,"  attr   = ");
                    int type_size = adios_type_size (vartype, value);
                    int nelems = attrsize / type_size;
                    char *p = (char*)value;
                    if (nelems>1) fprintf(outf,"{");
                    for (i=0; i<nelems; i++) { 
                        if (i>0) fprintf(outf,", ");
                        print_data(p, 0, vartype, false); 
                        p += type_size;
                    }
                    if (nelems>1) fprintf(outf,"}");
                    fprintf(outf,"\n");
                    matches = false; // already printed
                } else {
                    fprintf(outf,"  attr\n");
                }
            } else if (!vi) { 
                // after error
                fprintf(outf, "\n");
            } else if (vi->ndim > 0 || timed) {

                fprintf(outf,"  ");
                if (timed) 
                    fprintf(outf, "%d*", vi->nsteps);
                if (vi->ndim > 0) {
                    fprintf(outf,"{%" PRId64, vi->dims[0]);
                    for (j=1; j < vi->ndim; j++) {
                        fprintf(outf,", %" PRId64, vi->dims[j]);
                    }
                    fprintf(outf,"}");
                } else {
                    fprintf(outf,"scalar");
                }

                if (longopt && vi->statistics) {

                    if(timestep == false || timed == false ) {

                        fprintf(outf," = ");
                        if(vartype == adios_complex || vartype == adios_double_complex) {
                            // force printing (double,double) here
                            print_data(vi->statistics->min, 0, adios_double_complex, false); 
                            fprintf(outf," / ");
                            print_data(vi->statistics->max, 0, adios_double_complex, false); 
                            fprintf(outf," / ");
                            print_data(vi->statistics->avg, 0, adios_double_complex, false);
                            fprintf(outf," / ");
                            print_data(vi->statistics->std_dev, 0, adios_double_complex, false);
                        } else {
                            print_data(vi->statistics->min, 0, vartype, false); 
                            fprintf(outf," / ");
                            print_data(vi->statistics->max, 0, vartype, false); 
                            fprintf(outf," / ");
                            print_data(vi->statistics->avg, 0, adios_double, false);
                            fprintf(outf," / ");
                            print_data(vi->statistics->std_dev, 0, adios_double, false);
                        }

                        //fprintf(outf," {MIN / MAX / AVG / STD_DEV} ");
                    } 
                } // longopt && vi->statistics 
                fprintf(outf,"\n");

            } else {
                // scalar
                fprintf(outf,"  scalar");
                if (longopt && vi->value) {
                    fprintf(outf," = ");
                    print_data(vi->value, 0, vartype, false); 
                    matches = false; // already printed
                }
                fprintf(outf,"\n");
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

int doList(const char *path) 
{
    ADIOS_FILE  *fp;
    int     grpid;
    int     status;
    int     mpi_comm_dummy=0;
    int     verbose=0;
    char    init_params[128];
    int     adios_verbose=2;
  
    sprintf (init_params, "verbose=%d", adios_verbose);

    status = adios_read_init_method (ADIOS_READ_METHOD_BP, mpi_comm_dummy, init_params);
    if (status) {
        fprintf(stderr, "Error: %s\n", adios_errmsg());
        bpexit(6, 0);
    }

    // open the BP file
    fp = adios_read_open_file (path, ADIOS_READ_METHOD_BP, mpi_comm_dummy); 
    if (fp == NULL) {
        //fprintf(stderr, "Error: %s\n", adios_errmsg());
        bpexit(7, 0);
    }

    doList_group (fp);
    
    adios_read_close (fp);
    return 0;
}
