#include <stdint.h>
#include <inttypes.h>
#include "R_adios.h"


SEXP R_adios_init_noxml(SEXP R_comm){
  MPI_Comm comm;
  comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);
  
  adios_init_noxml(comm); //Calling adios_init_nomxl function 

  return(R_NilValue);

} /* End of R_adios_d_init_method(). */


SEXP R_adios_allocate_buffer(SEXP R_buffer_size){
// SEXP R_adios_allocate_buffer(SEXP R_adios_buffer_when, SEXP R_buffer_size){
// Later will add support for different methods
// Need to create enum 
// const char *buffer_when = CHARPT(R_adios_buffer_when, 0);
  
  uint64_t buffer_size;
  buffer_size   = (uint64_t) INTEGER(R_buffer_size)[0]; //Make sure this conv is correct ??
  adios_allocate_buffer(ADIOS_BUFFER_ALLOC_NOW, buffer_size);
 
  return(R_NilValue);

} /* End of R_adios_allocate_buffer(). */



SEXP R_adios_declare_group(SEXP R_adios_group_name, SEXP R_adios_time_index) {
  //Later will add suport for adios_flag_yes or adios_flag_no 

  const char *group_name = CHARPT(R_adios_group_name, 0);
  const char *time_index = CHARPT(R_adios_time_index, 0);

  SEXP R_m_adios_group;
  int64_t *m_adios_group;
  m_adios_group  = (int64_t*) malloc(sizeof(int64_t)); //Make sure this type ??

  PROTECT(R_m_adios_group = R_MakeExternalPtr(m_adios_group,
                                           R_NilValue, R_NilValue));

 //adios_declare_group (*m_adios_group, "restart", "", adios_flag_yes); // Returns group id   
  adios_declare_group (m_adios_group, group_name, time_index, adios_flag_yes); // ??

  UNPROTECT(1);
  return(R_m_adios_group);

}


SEXP R_adios_select_method(SEXP R_m_adios_group, SEXP R_adios_method, SEXP R_adios_params, SEXP R_adios_base_path ){
  
  //int64_t *group = INTEGER(R_m_adios_group); // Make sure ??
 
  int64_t *group;
  group = R_ExternalPtrAddr(R_m_adios_group); // Make sure ??

  const char *method = CHARPT(R_adios_method, 0);
  const char *params = CHARPT(R_adios_params, 0);
  const char *base_path = CHARPT(R_adios_base_path, 0);

  adios_select_method (*group, method, params, base_path);

  //free(group);
  return(R_NilValue);

}

//SEXP R_adios_define_var(SEXP R_m_adios_group, SEXP R_adios_varname, SEXP R_adios_path, SEXP R_adios_type, SEXP R_adios_local_dim, SEXP R_adios_global_dim, SEXP R_adios_local_offset){
SEXP R_adios_define_var(SEXP R_m_adios_group, SEXP R_adios_varname, SEXP R_adios_path, SEXP R_adios_local_dim, SEXP R_adios_global_dim, SEXP R_adios_local_offset){ 


  //const char *adios_type = CHARPT(R_adios_type, 0);
  //if(strcmp(adios_type,"adios_double") eq 0){
  //
  //}
   


  //int64_t *group = INTEGER(R_m_adios_group); // Make sure ?? 
  int64_t *group;
  group = R_ExternalPtrAddr(R_m_adios_group); // Make sure ?? 

  const char *varname = CHARPT(R_adios_varname, 0);
  const char *path = CHARPT(R_adios_path, 0);
  //for adios_type, we need to do enum hash lookup. For now we use "adios_double" ??"
  const char *local_dim = CHARPT(R_adios_local_dim, 0);
  const char *global_dim = CHARPT(R_adios_global_dim, 0);
  const char *local_offset = CHARPT(R_adios_local_offset, 0);

  R_debug_print("Calling adios_define_var function\n");
  R_debug_print("Value of group is %p\n",group);
  R_debug_print("The Direction is %p\n",&group);
  R_debug_print("Varname is %s\n",varname);
  R_debug_print("Path is %s\n",path);
  R_debug_print("Local dim is %s\n",local_dim);
  R_debug_print("Global dim is %s\n",global_dim);
  R_debug_print("Local_offset is %s\n",local_offset);
  // Add support for adios_double and adios_integer


  adios_define_var (*group, varname, path, adios_double, local_dim, global_dim, local_offset); // ?? will support for dynamic type as a input ?? Should "group" pass as a pointer or not ??   
   
  R_debug_print("Done adios_define_var function\n");

  //free(group);
   return(R_NilValue);
  //In future return 64bit ID of the definition that can be used to when writing multiple sub-blocks of the same variable within one process within one output step.
  
}

//SEXP R_adios_open(SEXP R_m_adios_file, SEXP R_adios_group_name, SEXP R_adios_file_name, SEXP R_adios_mode, SEXP R_comm){

SEXP R_adios_open(SEXP R_adios_group_name, SEXP R_adios_file_name, SEXP R_adios_mode, SEXP R_comm){ 

  R_debug_print("Calling R_adios_open function\n");
  SEXP R_m_adios_file;
  int64_t *m_adios_file; 
  m_adios_file = (int64_t*) malloc(sizeof(int64_t)); //Make sure this type ??              

  const char *group_name = CHARPT(R_adios_group_name, 0);
  const char *file_name = CHARPT(R_adios_file_name, 0);
  const char *mode = CHARPT(R_adios_mode, 0);

  MPI_Comm comm;
  comm = MPI_Comm_f2c(INTEGER(R_comm)[0]);

  PROTECT(R_m_adios_file = R_MakeExternalPtr(m_adios_file,
               R_NilValue, R_NilValue));

  //adios_open (*m_adios_file, group_name, file_name, mode, comm); // Calling adios_open 

  adios_open (m_adios_file, group_name, file_name, mode, comm); // ??

  UNPROTECT(1);
  return(R_m_adios_file);
}

SEXP R_adios_group_size(SEXP R_m_adios_file, SEXP R_adios_group_size){

  //int64_t *file_p = INTEGER(R_m_adios_file); // ??
  int64_t *file_p;
  file_p = R_ExternalPtrAddr(R_m_adios_file);

  uint64_t group_size = (uint64_t) INTEGER(R_adios_group_size)[0]; // ??

  R_debug_print("Group_size : %" PRIu64 "\n", group_size);
  

  SEXP R_adios_total_size;
  uint64_t *total_size;
  total_size = (uint64_t*) malloc(sizeof(uint64_t)); //Make sure this type ?? 

  R_debug_print("IN R_adios_group_size\n");

  PROTECT(R_adios_total_size = R_MakeExternalPtr(total_size,
             R_NilValue, R_NilValue));
  
  //adios_group_size (*file_p, group_size, *total_size);
  adios_group_size (*file_p, group_size, total_size); // Should pass file pointer or not(Manual and example are contradict) 
  
  UNPROTECT(1);
  return(R_adios_total_size); // Not sure where you want to use this value

}


SEXP R_adios_write(SEXP R_m_adios_file, SEXP R_adios_var_name, SEXP R_adios_var){

  //int64_t *file_p = INTEGER(R_m_adios_file); // ??
  int64_t *file_p;
  file_p = R_ExternalPtrAddr(R_m_adios_file);

  const char *var_name; 
  var_name = CHARPT(R_adios_var_name, 0);

  //void *var_address; 
  //var_address = R_ExternalPtrAddr(R_adios_var); // ?? 
  ////adios_write(file_p, var_name, (void *) &var_address); // Make sure ??
  
  int check;

  if(IS_INTEGER(R_adios_var)){
    int *int_var;
    int_var = INTEGER(R_adios_var);
    check = adios_write(*file_p, var_name, int_var);
  }else if(IS_NUMERIC(R_adios_var)){
    double *double_var;
    double_var = REAL(R_adios_var);
    check = adios_write(*file_p, var_name, double_var);
  }
  else{
    check = -1; //                                                      
  }
  
  R_debug_print("IN R_adios_write function call \n");

  //adios_write(*file_p, var_name, (void *) var_address);
  // free(var_address); 

  return(R_NilValue); // May want to send return "check" 
  
}


SEXP R_adios_close(SEXP R_m_adios_file){
  
  //int64_t *file_p = INTEGER(R_m_adios_file); // ?? 
  int64_t *file_p;
  file_p = R_ExternalPtrAddr(R_m_adios_file);

  R_debug_print("IN R_adios_close function call \n ");
  //adios_close(file_p);  // Need to pass as a pointer of not ??
   adios_close(*file_p); 

   //free(file_p);
  return(R_NilValue);

}

SEXP R_adios_finalize(SEXP R_comm_rank){
  R_debug_print("In R_adios_finalize\n");
  adios_finalize(INTEGER(R_comm_rank)[0]);
  return(R_NilValue);

} /* End of R_adios_finalize(). */



