#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "R_adios.h"
#include "adios_read.h"

#define INT(x) INTEGER(x)[0]
#define newRptr(ptr,Rptr,fin) PROTECT(Rptr = R_MakeExternalPtr(ptr, R_NilValue, R_NilValue));R_RegisterCFinalizerEx(Rptr, fin, TRUE)


static void finalizer(SEXP Rptr)
{
  void *ptr = (void *) R_ExternalPtrAddr(Rptr);
  if (NULL == ptr) return;
  
  free(ptr);
  R_ClearExternalPtr(Rptr);
}

//(For struct _ADIOS_FILE , struct _ADIOS_VARINFO, struct ADIOS_SELECTION)

// Make sure to implement error checking mechanism

//To construct look up table use public/adios_read_v2.h (Ask ADIOS team about adios_read_v2.h) 
int read_method_hash(const char *search_str){
  typedef struct read_method_table {
    const char *method_name;
    int val;
  } adios_rmt;
  adios_rmt table[] = {
    { "ADIOS_READ_METHOD_BP", 0},
    { "ADIOS_READ_METHOD_BP_AGGREGATE", 1},
    { "ADIOS_READ_METHOD_DATASPACES", 3},
    { "ADIOS_READ_METHOD_DIMES", 4},
    { "ADIOS_READ_METHOD_FLEXPATH", 5},
  };

  for (adios_rmt *i = table; i->method_name != NULL; i++) {
    if (strcmp(i->method_name, search_str) == 0) {
      return i->val;
    }
  }
  return -EINVAL;
} /*End of read_method_hash */


//To construct look up table use public/adios_read_v2.h (Ask ADIOS team about adios_read_v2.h)                          
int lock_mode_hash(const char *search_str){
  typedef struct lock_mode_table {
    const char *method_name;
    int val;
  } adios_lm;
  adios_lm table[] = {
    { "ADIOS_LOCKMODE_NONE", 0},
    { "ADIOS_LOCKMODE_CURRENT", 1},
    { "ADIOS_LOCKMODE_ALL", 2},
  };

  for (adios_lm *i = table; i->method_name != NULL; i++) {
    if (strcmp(i->method_name, search_str) == 0) {
      return i->val;
    }
  }
  return -EINVAL;
} /*End of lock_mode */


SEXP R_adios_read_init_method(SEXP R_adios_read_method, SEXP R_comm,
			      SEXP R_params){
  //since R has no enum construct. Get String from R and using lookup table send integer value to ADIOS
  //To construct look up table use public/adios_read_v2.h (Ask ADIOS team about adios_read_v2.h)

  int read_method_value=1111; //init dummy value
  const char *method_name = CHARPT(R_adios_read_method, 0); //Passing Char pointer to "read_method_hash" function.       
  read_method_value = read_method_hash(method_name); //Calling read_method_hash function

  MPI_Comm comm;
  comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);
  //adios_read_init_method(read_method_value, &comm, CHARPT(R_params, 0)); Do not need to pass address of "comm" anymore
  adios_read_init_method(read_method_value, comm, CHARPT(R_params, 0));  
  return(R_NilValue);
} /* End of R_adios_read_init_method(). */


SEXP R_adios_read_open(SEXP R_filename, SEXP R_adios_read_method, SEXP R_comm,
		       SEXP R_adios_lockmode, SEXP R_timeout_sec){
  char *filename;

  int read_method_value=1111; //init dummy value                           
  const char *read_method_name = CHARPT(R_adios_read_method, 0); //Passing Char pointer to "read_method_hash" function.
  read_method_value = read_method_hash(read_method_name); //Calling read_method_hash function    

  MPI_Comm comm;

  int lock_mode_value=1111; //init dummy value     
  const char *lock_method_name = CHARPT(R_adios_lockmode, 0); //Passing Char pointer to "lock_mode" function.                    
  lock_mode_value = lock_mode_hash(lock_method_name); //Calling read_method_hash function 

  double *timeout_sec;

  //return type is ADIOS_FILE *
  ADIOS_FILE *adios_file_ptr; 
  SEXP R_adios_file_ptr;

  filename = CHARPT(R_filename, 0);
  comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);
  timeout_sec = REAL(R_timeout_sec); //double check FLOAT vs DOUBLE in R 

  //BAKI make sure about structure
  //adios_file = (ADIOS_FILE *) malloc(sizeof(ADIOS_FILE));
  //R_adios_file  = adios_read_open (filename, read_method_value, &comm, lock_mode_value,timeout_sec);

  adios_file_ptr  = adios_read_open(filename, read_method_value, comm,
				    lock_mode_value,*timeout_sec);

  //  PROTECT(R_adios_file_ptr = R_MakeExternalPtr(adios_file_ptr, R_NilValue,
  //					       R_NilValue));
  newRptr(adios_file_ptr, R_adios_file_ptr, finalizer);
  UNPROTECT(1);

  return(R_adios_file_ptr);
}

SEXP R_adios_inq_var(SEXP R_adios_file_ptr, SEXP R_adios_varname){
  ADIOS_FILE * fp;
  fp = R_ExternalPtrAddr(R_adios_file_ptr);

  ADIOS_VARINFO *adios_var_info;
  SEXP R_adios_var_info;

  adios_var_info = adios_inq_var(fp,CHARPT(R_adios_varname, 0));

  //  PROTECT(R_adios_var_info = R_MakeExternalPtr(adios_var_info, R_NilValue,
  //					       R_NilValue));
  newRptr(adios_var_info, R_adios_var_info, finalizer);
  UNPROTECT(1);
  return(R_adios_var_info);	 
}

SEXP R_custom_inq_var_ndim(R_adios_var_info){
  ADIOS_VARINFO *adios_var_info;
  adios_var_info = R_ExternalPtrAddr(R_adios_var_info);

  SEXP R_custom_inq_var_ndim_val = PROTECT(allocVector(INTSXP, 1));
  INTEGER (R_custom_inq_var_ndim_val)[0] = adios_var_info -> ndim; 
  //printf("In C ndim=%d \n", ndim_val);
  UNPROTECT(1);

  return(R_custom_inq_var_ndim_val);
}

SEXP R_custom_inq_var_dims(R_adios_var_info){
  ADIOS_VARINFO *adios_var_info;
  adios_var_info = R_ExternalPtrAddr(R_adios_var_info);

  SEXP R_custom_inq_var_dims_val = PROTECT(allocVector(INTSXP,
						       adios_var_info -> ndim));
  for(int i=0;i<adios_var_info -> ndim;i++){

    INTEGER (R_custom_inq_var_dims_val)[i] = adios_var_info -> dims[i];
    //INTEGER (R_custom_inq_var_dims_val)[0] = adios_var_info -> dims[0];
    //INTEGER (R_custom_inq_var_dims_val)[1] = adios_var_info -> dims[1];
  }

  //printf("In C ndim=%d \n", ndim_val);                          
          
  UNPROTECT(1);
  return(R_custom_inq_var_dims_val);
}

SEXP R_adios_inq_var_blockinfo(SEXP R_adios_file_ptr, SEXP R_adios_var_info){
  ADIOS_FILE *fp;
  fp = R_ExternalPtrAddr(R_adios_file_ptr);

  ADIOS_VARINFO *adios_var_info;
  adios_var_info = R_ExternalPtrAddr(R_adios_var_info);

  adios_inq_var_blockinfo(fp, adios_var_info);

  return(R_NilValue);
}

SEXP R_adios_selection_bounding_box(SEXP R_adios_ndim, SEXP R_adios_start,
				    SEXP R_adios_count){
  // Ask norbert for malloc size = ndim is ok or not 
  int *ndim;
  int *start;
  int *count;

  R_adios_start = coerceVector(R_adios_start, INTSXP);
  R_adios_count = coerceVector(R_adios_count, INTSXP);

  ndim = INTEGER(R_adios_ndim);
  //start = (uint64_t *) INTEGER(R_adios_start);
  //count = (uint64_t *) INTEGER(R_adios_count);

  start = INTEGER(R_adios_start);
  count = INTEGER(R_adios_count);

  //Malloc  
  uint64_t *start_adios =  malloc( (*ndim) * sizeof(uint64_t));
  uint64_t *count_adios =  malloc( (*ndim) * sizeof(uint64_t));
  //Copy from start to start_adios and count to count_adios
  for (int pos = 0; pos < *ndim; pos++) { 
    start_adios[pos] = start[pos];
    count_adios[pos] = count[pos];
  }

  ADIOS_SELECTION *adios_selection;
  SEXP R_adios_selection;

  //printf("Value of Start 0 => %d\n", start_adios[0]);
  //printf("Value of Start 1 => %d\n", start_adios[1]);

  //printf("Value of Count 0 => %d\n", count_adios[0]);
  //printf("Value of Count 1 => %d\n", count_adios[1]);


  adios_selection = adios_selection_boundingbox(*ndim, start_adios, count_adios);
  //  PROTECT(R_adios_selection = R_MakeExternalPtr(adios_selection, R_NilValue,
  //						R_NilValue));
  newRptr(adios_selection, R_adios_selection, finalizer);
  UNPROTECT(1);
  return(R_adios_selection);
}

SEXP R_adios_schedule_read(SEXP R_adios_var_info, SEXP R_adios_start,
			   SEXP R_adios_count, SEXP R_adios_file_ptr,
			   SEXP R_adios_selection, SEXP R_adios_varname,
			   SEXP R_adios_from_steps, SEXP R_adios_nsteps){
  ADIOS_VARINFO *adios_var_info;
  adios_var_info = R_ExternalPtrAddr(R_adios_var_info);

  // Ask norbert for malloc size = ndim is ok or not                      
  uint64_t ndim = adios_var_info -> ndim;
  int *start;
  int *count;

  R_adios_start = coerceVector(R_adios_start, INTSXP);
  R_adios_count = coerceVector(R_adios_count, INTSXP);

  //ndim = (uint64_t *) INTEGER(R_adios_ndim);
  //start = (uint64_t *) INTEGER(R_adios_start);      
  //count = (uint64_t *) INTEGER(R_adios_count); 
 
  start = INTEGER(R_adios_start);
  count = INTEGER(R_adios_count);

  //Malloc                         
  //uint64_t *start_adios =  malloc( (*ndim) * sizeof(uint64_t));
  //uint64_t *count_adios =  malloc( (*ndim) * sizeof(uint64_t));
  //Copy from start to start_adios and count to count_adios              

  uint64_t *start_adios =  malloc( (adios_var_info -> ndim)*sizeof(uint64_t));  
  uint64_t *count_adios =  malloc( (adios_var_info -> ndim)*sizeof(uint64_t));  

  //for (int pos = 0; pos < *ndim; pos++) {
  for (int pos = 0; pos < adios_var_info -> ndim; pos++) {
    start_adios[pos] = start[pos];
    count_adios[pos] = count[pos];
  }


  //const char *datatype = CHARPT(R_adios_datatype,0);
  //int datasize = 0;
  //if(strcmp(datatype, "integer") == 0) {
  //datasize = 4;
    //}
    //else if(strcmp(datatype, "double") == 0) {
    //datasize = 8;
  //}
  //else{
  //datasize = 0;
  //}
  // add more datatypes here such as float, long int etc..

  int datasize = adios_type_size(adios_var_info -> type, adios_var_info->value);

  SEXP R_adios_data;
  ADIOS_FILE * fp;
  fp = R_ExternalPtrAddr(R_adios_file_ptr);

  ADIOS_SELECTION *adios_selection;
  adios_selection = R_ExternalPtrAddr(R_adios_selection);

  const char *varname = CHARPT(R_adios_varname, 0);

  int *from_steps = INTEGER(R_adios_from_steps);
  int *nsteps = INTEGER(R_adios_nsteps);

  void *adios_data;
  uint64_t ndata = 1;
  for (int dim = 0; dim < ndim; dim++)
    ndata *= count[dim];
  //adios_data = (void *) malloc(sizeof(adios_data));
  adios_data = malloc(ndata * datasize);
  adios_schedule_read(fp, adios_selection, varname, *from_steps,
		      *nsteps,adios_data); 
  //  PROTECT(R_adios_data = R_MakeExternalPtr(adios_data, R_NilValue, R_NilValue));
  newRptr(adios_data, R_adios_data, finalizer);
  UNPROTECT(1);
  return(R_adios_data);
}

SEXP R_custom_data_access(SEXP R_adios_data, SEXP R_adios_selection,
			  SEXP R_adios_var_info){
  void *adios_data;
  adios_data = R_ExternalPtrAddr(R_adios_data);

  ADIOS_SELECTION *adios_selection;
  adios_selection = R_ExternalPtrAddr(R_adios_selection);

  //const char *datatype = CHARPT(R_adios_datatype,0);
  //int datasize = 0;

  ADIOS_VARINFO *adios_var_info;
  adios_var_info = R_ExternalPtrAddr(R_adios_var_info);
  int data_type_size = adios_type_size(adios_var_info -> type,
				       adios_var_info -> value);
  int num_element = 1;

  const char *data_type_string = adios_type_to_string(adios_var_info -> type);

  SEXP R_custom_data_access_val; 

  for (int pos = 0; pos < adios_var_info -> ndim; pos++) {
    num_element = num_element * adios_selection -> u.bb.count[pos];
  }

  //Check all these sizes again IMP. What to do INT VS FLAOT
  
  //if(strcmp(datatype, "integer") == 0) {
    //cast to integer
  if(strcmp(data_type_string,"integer") == 0){
    printf("Error found in R_custom_data_access\n");
  //R_custom_data_access_val = PROTECT(allocVector(INTSXP, 1));
  //  int *data = (int *) adios_data;
  //  int data_val = *(data + (*adios_dataindex));
  //  INTEGER(R_custom_data_access_val)[0] = data_val;
  }
  //else if(strcmp(datatype, "double") == 0) {
    //cast to double
  else if(strcmp(data_type_string,"real") ==0){
    R_custom_data_access_val = PROTECT(allocVector(REALSXP, num_element));
    float *data = (float *) adios_data;
    //float data_val = *(data + (*adios_dataindex));    
    for(int i=0;i<num_element;i++){
      REAL(R_custom_data_access_val)[i] = *(data + i);
    }
    //REAL(R_custom_data_access_val) = data_val;
  }
  else if(strcmp(data_type_string,"double") ==0){
    R_custom_data_access_val = PROTECT(allocVector(REALSXP, num_element));
    double *data = (double *) adios_data;
    //double data_val = *(data + (*adios_dataindex));
  
    for(int i=0;i<num_element;i++){
      REAL(R_custom_data_access_val)[i] = *(data + i);
    }
    //REAL(R_custom_data_access_val)[0] = data_val;
  }
  else{
    printf("Error found in R_custom_data_access\n");
  }

  UNPROTECT(1);
  return(R_custom_data_access_val); 
}

SEXP R_adios_perform_reads(SEXP R_adios_file_ptr, SEXP R_adios_blocking){
  SEXP ret;
  PROTECT(ret = allocVector(INTSXP, 1));
  
  ADIOS_FILE *fp;
  fp = R_ExternalPtrAddr(R_adios_file_ptr);
  
  printf("Just before adios_perform_reads\n");
  INT(ret) = adios_perform_reads(fp, INT(R_adios_blocking));
  return ret;
}

SEXP R_adios_advance_step(SEXP R_adios_file_ptr, SEXP R_adios_last,
			  SEXP R_adios_timeout_sec){
  SEXP ret;
  PROTECT(ret = allocVector(INTSXP, 1));
  
  ADIOS_FILE *fp;
  fp = R_ExternalPtrAddr(R_adios_file_ptr);
  
  int *last;
  last = INTEGER(R_adios_last);
  
  double *timeout_sec;
  timeout_sec = REAL(R_adios_timeout_sec);
  
  INT(ret) = adios_advance_step(fp, INT(R_adios_last),
				REAL(R_adios_timeout_sec)[0]);
  return ret;
}

SEXP R_adios_read_close(SEXP R_adios_file_ptr){
  ADIOS_FILE * fp;
  fp = R_ExternalPtrAddr(R_adios_file_ptr);
  adios_read_close(fp);
  return(R_NilValue);
}

SEXP R_adios_read_finalize_method(SEXP R_adios_read_method){
  int read_method_value=1111; //init dummy value                          
  const char *method_name = CHARPT(R_adios_read_method, 0); //Passing Char pointer to "read_method_hash" function.                                    
  read_method_value = read_method_hash(method_name); //Calling read_method\ hash function  
  adios_read_finalize_method(read_method_value);
  return(R_NilValue);
}

SEXP R_adios_errno(){
  SEXP R_adios_errno_val = PROTECT(allocVector(INTSXP, 1));
  INTEGER(R_adios_errno_val)[0] = adios_errno;
  //printf("In C ndim=%d \n", adios_errno);
  UNPROTECT(1);
  return(R_adios_errno_val);
}

