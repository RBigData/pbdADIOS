#ifndef __R_DUMP__
#define __R_DUMP__

#define MAX_DIMS 16
#define MAX_MASKS 10
#define MAX_BUFFERSIZE (10*1024*1024)

int getTypeInfo( enum ADIOS_DATATYPES adiosvartype, int* elemsize);

void print_endline(void);
int print_dataset(void *data, enum ADIOS_DATATYPES adiosvartype, 
        uint64_t *s, uint64_t *c, int tdims, int *ndigits);
int readVar(ADIOS_FILE *fp, ADIOS_VARINFO *vi, const char * name, bool timed);
int dump_vars (ADIOS_FILE *fp);

SEXP R_dump(SEXP R_adios_path,
            SEXP R_comm,
            SEXP R_adios_rank);

#endif