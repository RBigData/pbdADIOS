#include "R_bpls.h"
#include "R_dump.h"

static int nextcol = 0;
int  ncols1 = 6; // how many values to print in one row (only for -p)

static inline int ndigits (int n) 
{
    static char digitstr[32];
    return snprintf (digitstr, 32, "%d", n);
}

#define PRINT_DIMS64(str, v, n, loopvar) Rprintf("%s = { ", str); \
    for (loopvar=0; loopvar<n;loopvar++) Rprintf("%" PRId64 " ", v[loopvar]);    \
printf("}")

/**
 * R wrapper of dump
 */
SEXP R_dump(SEXP R_adios_path,
            SEXP R_comm,
            SEXP R_adios_rank)
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
        dump_vars (fp);
    
    adios_read_close (fp);
    adios_read_finalize_method(ADIOS_READ_METHOD_BP);

    return R_NilValue;
}

/**
 * dump vars
 */
int dump_vars (ADIOS_FILE *fp)
{
    ADIOS_VARINFO *vi; 
    ADIOS_VARINFO **vis; 
    enum ADIOS_DATATYPES vartype;
    int     i, j, n;             // loop vars

    int     retval;
    int     nNames; // number of vars + attrs
    bool    timed;  // variable has multiple timesteps

    nNames = fp->nvars;

    vis = (ADIOS_VARINFO **) malloc (nNames * sizeof (ADIOS_VARINFO*));
    if (vis == NULL) {
        REprintf("Error: could not allocate %d elements\n", nNames);
        return 5;
    }

    //names = fp-var_namelist

    for (n=0; n<nNames; n++) {
        vis[n] = adios_inq_var (fp, fp->var_namelist[n]);
        if (!vis[n]) {
            REprintf("Error: %s\n", adios_errmsg());
        }
    }

    /* VARIABLES */
    for (n=0; n<nNames; n++) {

        vi = vis[n];
        vartype = vi->type;
        //timed = adios_read_bp_is_var_timed(fp, vi->varid);
        timed = (vi->nsteps > 1);

        retval = readVar(fp, vi, fp->var_namelist[n], timed);
        if (retval && retval != 10) // do not return after unsupported type
            return retval;
        Rprintf("\n");

    }
    /* Free ADIOS_VARINFOs */
    for (n=0; n<nNames; n++) {
        adios_free_varinfo(vis[n]);
    }

    return 0;
} 

int getTypeInfo( enum ADIOS_DATATYPES adiosvartype, int* elemsize)
{
    switch(adiosvartype) {
        case adios_unsigned_byte:
            *elemsize = 1;
            break;
        case adios_byte:
            *elemsize = 1;
            break;
        case adios_string:
            *elemsize = 1;
            break;

        case adios_unsigned_short:  
            *elemsize = 2;
            break;
        case adios_short:
            *elemsize = 2;
            break;

        case adios_unsigned_integer:
            *elemsize = 4;
            break;
        case adios_integer:    
            *elemsize = 4;
            break;

        case adios_unsigned_long:
            *elemsize = 8;
            break;
        case adios_long:        
            *elemsize = 8;
            break;

        case adios_real:
            *elemsize = 4;
            break;

        case adios_double:
            *elemsize = 8;
            break;

        case adios_complex:  
            *elemsize = 8;
            break;

        case adios_double_complex:
            *elemsize = 16;
            break;

        case adios_long_double: // do not know how to print
            //*elemsize = 16;
        default:
            return 1;
    }
    return 0;
}

/** 
 * Read data of a variable and print 
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
    
    int  istart[MAX_DIMS], icount[MAX_DIMS];
    int  verbose = 0;
    for (i=0; i<MAX_DIMS; i++) {
        istart[i]  = 0;
        icount[i]  = -1;  // read full var by default
    }

    if (getTypeInfo(vi->type, &elemsize)) {
        REprintf("Adios type %d (%s) not supported in bpls. var=%s\n", 
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
            REprintf("Error when scheduling variable %s for reading. errno=%d : %s \n", name, adios_errno, adios_errmsg());
            free(data);
            return 11;
        }

        status = adios_perform_reads (fp, 1); // blocking read performed here
        if (status < 0) {
            REprintf("Error when reading variable %s. errno=%d : %s \n", name, adios_errno, adios_errmsg());
            free(data);
            return 11;
        }

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
        Rprintf("\n");
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
            if (tdims > 0) {
                sprintf(idxstr,"    (%*" PRId64,ndigits[0], ids[0]);
                for (i=1; i<tdims; i++) {
                    sprintf(buf,",%*" PRId64,ndigits[i],ids[i]);
                    strcat(idxstr, buf);
                }
                strcat(idxstr,")    ");
            }
        }

        // print item
        Rprintf("%s", idxstr);
        print_data(data, item, adiosvartype);

        // increment/reset column index
        nextcol++;
        if (nextcol == ncols1) {
            Rprintf("\n");
            nextcol = 0;
        } else {
            Rprintf(" ");
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
