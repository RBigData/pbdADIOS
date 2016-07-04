#include "R_bpls.h"

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

    if(!rank)
        doList_group (fp);
    
    adios_read_close (fp);
    adios_read_finalize_method(ADIOS_READ_METHOD_BP);

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

                adios_inq_var_stat (fp, vi, timestep && timed, 0);

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

        /*if (matches && dump) {
            // print variable content 
            if (isVar[n])
                retval = readVar(fp, vi, names[n], timed);
            if (retval && retval != 10) // do not return after unsupported type
                return retval;
            fprintf(outf,"\n");
        }*/

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

/** Read data of a variable and print 
 * Return: 0: ok, != 0 on error
 */
int readVar(ADIOS_FILE *fp, ADIOS_VARINFO *vi, const char * name, bool timed)
{
    int i,j;
    uint64_t start_t[MAX_DIMS], count_t[MAX_DIMS]; // processed <0 values in start/count
    uint64_t s[MAX_DIMS], c[MAX_DIMS]; // for block reading of smaller chunks
    int tdims;               // number of dimensions including time
    int tidx;                // 0 or 1 to account for time dimension
    uint64_t nelems;         // number of elements to read
    int elemsize;            // size in bytes of one element
    uint64_t st, ct;
    void *data;
    uint64_t sum;           // working var to sum up things
    int  maxreadn;          // max number of elements to read once up to a limit (10MB of data)
    int  actualreadn;       // our decision how much to read at once
    int  readn[MAX_DIMS];   // how big chunk to read in in each dimension?
    int  status;            
    bool incdim;            // used in incremental reading in
    ADIOS_SELECTION * sel;  // boundnig box to read
    int ndigits_dims[32];        // # of digits (to print) of each dimension 

    if (getTypeInfo(vi->type, &elemsize)) {
        fprintf(stderr, "Adios type %d (%s) not supported in bpls. var=%s\n", 
                vi->type, adios_type_to_string(vi->type), name);
        return 10;
    }

    // create the counter arrays with the appropriate lengths
    // transfer start and count arrays to format dependent arrays

    nelems = 1;
    tidx = 0;

    if (timed) {
        if (istart[0] < 0)  // negative index means last-|index|
            st = vi->nsteps+istart[0];
        else
            st = istart[0];
        if (icount[0] < 0)  // negative index means last-|index|+1-start
            ct = vi->nsteps+icount[0]+1-st;
        else
            ct = icount[0];

        if (verbose>2) 
            printf("    j=0, st=%" PRIu64 " ct=%" PRIu64 "\n", st, ct);

        start_t[0] = st;
        count_t[0] = ct;
        nelems *= ct;
        if (verbose>1) 
            printf("    s[0]=%" PRIu64 ", c[0]=%" PRIu64 ", n=%" PRIu64 "\n",
                    start_t[0], count_t[0], nelems);
        
        tidx = 1;
    }
    tdims = vi->ndim + tidx;

    for (j=0; j<vi->ndim; j++) {
        if (istart[j+tidx] < 0)  // negative index means last-|index|
            st = vi->dims[j]+istart[j+tidx];
        else
            st = istart[j+tidx];
        if (icount[j+tidx] < 0)  // negative index means last-|index|+1-start
            ct = vi->dims[j]+icount[j+tidx]+1-st;
        else
            ct = icount[j+tidx];

        if (verbose>2) 
            printf("    j=%d, st=%" PRIu64 " ct=%" PRIu64 "\n", j+tidx, st, ct);

        start_t[j+tidx] = st;
        count_t[j+tidx] = ct;
        nelems *= ct;
        if (verbose>1) 
            printf("    s[%d]=%" PRIu64 ", c[%d]=%" PRIu64 ", n=%" PRIu64 "\n",
                    j+tidx, start_t[j+tidx], j+tidx, count_t[j+tidx], nelems);
    }

    if (verbose>1) {
        printf(" total size of data to read = %" PRIu64 "\n", nelems*elemsize);
    }

    //print_slice_info(vi->ndim, vi->dims, timed, vi->nsteps, start_t, count_t);

    maxreadn = MAX_BUFFERSIZE/elemsize;
    if (nelems < maxreadn)
        maxreadn = nelems;

    // special case: string. Need to use different elemsize
    if (vi->type == adios_string) {
        if (vi->value)
            elemsize = strlen(vi->value)+1;
        maxreadn = elemsize;
    }

    // allocate data array
    data = (void *) malloc (maxreadn*elemsize+8); // +8 for just to be sure

    // determine strategy how to read in:
    //  - at once
    //  - loop over 1st dimension
    //  - loop over 1st & 2nd dimension
    //  - etc
    if (verbose>1) printf("Read size strategy:\n");
    sum = (uint64_t) 1;
    actualreadn = (uint64_t) 1;
    for (i=tdims-1; i>=0; i--) {
        if (sum >= (uint64_t) maxreadn) {
            readn[i] = 1;
        } else {
            readn[i] = maxreadn / (int)sum; // sum is small for 4 bytes here
            // this may be over the max count for this dimension
            if (readn[i] > count_t[i]) 
                readn[i] = count_t[i];
        }
        if (verbose>1) printf("    dim %d: read %d elements\n", i, readn[i]);
        sum = sum * (uint64_t) count_t[i];
        actualreadn = actualreadn * readn[i];
    }
    if (verbose>1) printf("    read %d elements at once, %" PRId64 " in total (nelems=%" PRId64 ")\n", actualreadn, sum, nelems);


    // init s and c
    // and calculate ndigits_dims
    for (j=0; j<tdims; j++) {
        s[j]=start_t[j];
        c[j]=readn[j];

        ndigits_dims[j] = ndigits (start_t[j]+count_t[j]-1); // -1: dim=100 results in 2 digits (0..99)
    }

    // read until read all 'nelems' elements
    sum = 0;
    while (sum < nelems) {

        // how many elements do we read in next?
        actualreadn = 1;
        for (j=0; j<tdims; j++) 
            actualreadn *= c[j];

        if (verbose>2) {
            printf("adios_read_var name=%s ", name);
            PRINT_DIMS64("  start", s, tdims, j); 
            PRINT_DIMS64("  count", c, tdims, j); 
            printf("  read %d elems\n", actualreadn);
        }

        // read a slice finally
        sel = adios_selection_boundingbox (vi->ndim, s+tidx, c+tidx);
        if (timed) {
            status = adios_schedule_read_byid (fp, sel, vi->varid, s[0], c[0], data); 
        } else {
            status = adios_schedule_read_byid (fp, sel, vi->varid, 0, 1, data); 
        }

        if (status < 0) {
            fprintf(stderr, "Error when scheduling variable %s for reading. errno=%d : %s \n", name, adios_errno, adios_errmsg());
            free(data);
            return 11;
        }

        status = adios_perform_reads (fp, 1); // blocking read performed here
        if (status < 0) {
            fprintf(stderr, "Error when reading variable %s. errno=%d : %s \n", name, adios_errno, adios_errmsg());
            free(data);
            return 11;
        }

        //if (verbose>2) printf("  read %" PRId64 " bytes\n", bytes_read);

        // print slice
        print_dataset(data, vi->type, s, c, tdims, ndigits_dims); 

        // prepare for next read
        sum += actualreadn;
        incdim=true; // largest dim should be increased 
        for (j=tdims-1; j>=0; j--) {
            if (incdim) {
                if (s[j]+c[j] == start_t[j]+count_t[j]) {
                    // reached the end of this dimension
                    s[j] = start_t[j];
                    c[j] = readn[j];
                    incdim = true; // next smaller dim can increase too
                } else {
                    // move up in this dimension up to total count
                    s[j] += readn[j];
                    if (s[j]+c[j] > start_t[j]+count_t[j]) {
                        // do not reach over the limit
                        c[j] = start_t[j]+count_t[j]-s[j];
                    }
                    incdim = false;
                }
            }
        }
    } // end while sum < nelems
    print_endline();


    free(data);
    return 0;
}

void print_endline(void) 
{
    if (nextcol != 0)
        fprintf(outf,"\n");
    nextcol = 0;
}

int print_dataset(void *data, enum ADIOS_DATATYPES adiosvartype, 
        uint64_t *s, uint64_t *c, int tdims, int *ndigits)
{
    int i,item, steps;
    char idxstr[128], buf[16];
    uint64_t ids[MAX_DIMS];  // current indices
    bool roll;

    // init current indices
    steps = 1;
    for (i=0; i<tdims; i++) {
        ids[i] = s[i];
        steps *= c[i];
    }

    item = 0; // index to *data 
    // loop through each data item and print value
    while (item < steps) {

        // print indices if needed into idxstr;
        idxstr[0] = '\0'; // empty idx string
        if (nextcol == 0) {
            if (!noindex && tdims > 0) {
                sprintf(idxstr,"    (%*" PRId64,ndigits[0], ids[0]);
                for (i=1; i<tdims; i++) {
                    sprintf(buf,",%*" PRId64,ndigits[i],ids[i]);
                    strcat(idxstr, buf);
                }
                strcat(idxstr,")    ");
            }
        }

        // print item
        fprintf(outf, "%s", idxstr);
        if (printByteAsChar && (adiosvartype == adios_byte || adiosvartype == adios_unsigned_byte)) {
            /* special case: k-D byte array printed as (k-1)D array of strings */
            if (tdims == 0) {
                print_data_as_string(data, steps, adiosvartype);
            } else {
                print_data_as_string(data+item, c[tdims-1], adiosvartype); // print data of last dim as string
                item += c[tdims-1]-1; // will be ++-ed once below
                ids[tdims-1] = s[tdims-1]+c[tdims-1]-1; // will be rolled below
            }
            nextcol = ncols-1; // force new line, will be ++-ed once below
        } else {
            print_data(data, item, adiosvartype, true);
        }

        // increment/reset column index
        nextcol++;
        if (nextcol == ncols) {
            fprintf(outf,"\n");
            nextcol = 0;
        } else {
            fprintf(outf," ");
        }

        // increment indices
        item++;
        roll = true;
        for (i=tdims-1; i>=0; i--) {
            if (roll) {
                if (ids[i] == s[i]+c[i]-1 ) {
                    // last index in this dimension, roll upward
                    ids[i] = s[i];
                } else {
                    ids[i]++;
                    roll = false;
                }
            }
        }
    }
    return 0;
}