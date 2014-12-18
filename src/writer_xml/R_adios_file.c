/* ADIOS file functions. */

#include "R_adios.h"

SEXP R_adios_open(SEXP R_filename, SEXP R_comm){
	char *filename;
	MPI_Comm comm;
	int64_t *adios_handle;
	SEXP R_adios_handle;

	filename = CHARPT(R_filename, 0);
	comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);	
	adios_handle = (int64_t*) malloc(sizeof(int64_t));

	PROTECT(R_adios_handle = R_MakeExternalPtr(adios_handle,
					R_NilValue, R_NilValue));
	adios_open(adios_handle, "arrays", filename, "r", comm);

	UNPROTECT(1);
	return(R_adios_handle);
} /* End of R_adios_open(). */

SEXP R_adios_close(SEXP R_adios_handle){
	int64_t *adios_handle;

	adios_handle = R_ExternalPtrAddr(R_adios_handle);
	adios_close(*adios_handle);

	free(adios_handle);
	return(R_NilValue);
} /* End of R_adios_close(). */

