#ifndef __R_STAGE_READ__
#define __R_STAGE_READ__

void slice(uint64_t length, uint64_t *s, uint64_t *e, int rank, int mpisize);

int schedule_stage_read (ADIOS_FILE * fp, 
                         const char *varname,
                         void ** data,
                         ADIOS_SELECTION ** sel,
                         ADIOS_VARINFO ** vi,
                         uint64_t p,   //number of ranks
                         uint64_t rank);

SEXP R_stage_read(SEXP R_adios_file_ptr,
                  SEXP R_varname,
                  SEXP R_nvars,
                  SEXP R_comm,
                  SEXP R_p,
                  SEXP R_adios_rank);

#endif