/* ADIOS file functions. */

#include "R_adios.h"

SEXP R_adios_write(SEXP R_adios_handle, SEXP R_var_name, SEXP R_var){
	int64_t *adios_handle;
	char *var_name;
        int ret;
	var_name = CHARPT(R_var_name, 0);
	adios_handle = R_ExternalPtrAddr(R_adios_handle);

	if(IS_INTEGER(R_var)){
	  int *int_var;
	  int_var = INTEGER(R_var);
	  ret = adios_write(adios_handle, var_name, int_var);
        }else if(IS_NUMERIC(R_var)){
	  double *double_var;
          double_var = REAL(R_var);
          ret = adios_write(adios_handle, var_name, double_var);
	} 
	else{
	  ret = -1; // 
        }
	return(AsInt(ret));
} /* End of R_adios_write(). */ 


#init
#open
#group_size
#adios_write
#adios_close
#finalize
