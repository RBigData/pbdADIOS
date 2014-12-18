/* ADIOS utility functions. */

#include "R_adios.h"

SEXP R_adios_init(SEXP R_filename, SEXP R_comm){
        MPI_Comm comm;
        comm = MPI_Comm_f2c(INTEGER(R_comm)[0]); 
 
        adios_init(CHARPT(R_filename, 0), comm);

	return(R_NilValue);

} /* End of R_adios_init(). */

SEXP R_adios_finalize(SEXP R_comm_rank){
	adios_finalize(INTEGER(R_comm_rank)[0]);
	return(R_NilValue);
} /* End of R_adios_finalize(). */

SEXP AsInt(int x){
  SEXP R_x;

  PROTECT(R_x = allocVector(INTSXP, 1));
  INTEGER(R_x)[0] = x;
  UNPROTECT(1);

  return(R_x);
} /* End of AsInt(). */
