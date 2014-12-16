/* ADIOS file functions. */

#include "R_adios.h"

SEXP R_adios_group_size(SEXP R_adios_handle, SEXP R_adios_groupsize, SEXP R_adios_totalsize){

	int64_t *adios_handle;
	uint64_t    adios_groupsize, adios_totalsize;
        SEXP R_ret;

	adios_handle = R_ExternalPtrAddr(R_adios_handle);
	//adios_groupsize = R_ExternalPtrAddr(R_adios_groupsize);
	//adios_totalsize = R_ExternalPtrAddr(R_adios_totalsize);
	
	adios_groupsize = (uint64_t) INTEGER(R_adios_groupsize)[0];
	adios_totalsize = (uint64_t) INTEGER(R_adios_totalsize)[0];
	
	PROTECT(R_ret = allocVector(INTSXP,2));
	
	Rprintf("=======A=======\n");
	Rprintf("value of adios_GROUPsize is %lu\n",adios_groupsize);
	Rprintf("value of adios_TOTALsize is %lu\n",adios_totalsize);
	Rprintf("=======B=======\n");

	// Need to have Double for adios_totalsize ??
	INTEGER(R_ret)[0] = adios_group_size(*adios_handle, adios_groupsize, &adios_totalsize);
	INTEGER(R_ret)[1] = (int) adios_totalsize; 
	
	UNPROTECT(1);
	
	Rprintf("BEFORE RETURN in R_adios_group_size\n");

	return(R_ret);
} /* End of R_adios_group_size(). */ 


