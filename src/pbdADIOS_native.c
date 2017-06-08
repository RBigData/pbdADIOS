/* Automatically generated. Do not edit by hand. */
  
#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <stdlib.h>

extern SEXP R_adios_advance_step(SEXP R_adios_file_ptr, ;
extern SEXP R_adios_allocate_buffer(SEXP R_adios_buffer_when, ;
extern SEXP R_adios_attr_read(SEXP R_adios_rank,;
extern SEXP R_adios_close(SEXP R_m_adios_file);
extern SEXP R_adios_declare_group(SEXP R_adios_group_name, ;
extern SEXP R_adios_define_var(SEXP R_m_adios_group, ;
extern SEXP R_adios_errno();
extern SEXP R_adios_finalize(SEXP R_comm_rank);
extern SEXP R_adios_free_varinfo (SEXP R_adios_var_info);
extern SEXP R_adios_get_attr(SEXP R_adios_fp, ;
extern SEXP R_adios_get_attr_byid(SEXP R_adios_fp, ;
extern SEXP R_adios_group_size(SEXP R_m_adios_file, ;
extern SEXP R_adios_init_noxml(SEXP R_comm);
extern SEXP R_adios_inq_var(SEXP R_adios_file_ptr, ;
extern SEXP R_adios_inq_var_blockinfo(SEXP R_adios_file_ptr, ;
extern SEXP R_adios_inq_var_byid(SEXP R_adios_file_ptr, ;
extern SEXP R_adios_open(SEXP R_adios_group_name, ;
extern SEXP R_adios_perform_reads(SEXP R_adios_file_ptr, ;
extern SEXP R_adios_print_attr(SEXP R_adios_rank,;
extern SEXP R_adios_read_close(SEXP R_adios_file_ptr);
extern SEXP R_adios_read_finalize_method(SEXP R_adios_read_method);
extern SEXP R_adios_read_init_method(SEXP R_adios_read_method,;
extern SEXP R_adios_read_open(SEXP R_filename, ;
extern SEXP R_adios_release_step(SEXP R_adios_file_ptr);
extern SEXP R_adios_schedule_read(SEXP R_adios_var_info, ;
extern SEXP R_adios_select_method(SEXP R_m_adios_group, ;
extern SEXP R_adios_selection_boundingbox(SEXP R_adios_ndim, ;
extern SEXP R_adios_set_max_buffer_size(SEXP R_max_buffer_size_MB);
extern SEXP R_adios_var_attr_read(SEXP R_adios_rank,;
extern SEXP R_adios_write(SEXP R_m_adios_file, ;
extern SEXP R_append(SEXP R_filename,;
extern SEXP R_bpls(SEXP R_adios_path,;
extern SEXP R_create(SEXP R_groupname,;
extern SEXP R_custom_data_access(SEXP R_adios_data, ;
extern SEXP R_custom_inq_var_dims(SEXP R_adios_var_info);
extern SEXP R_custom_inq_var_ndim(SEXP R_adios_var_info);
extern SEXP R_define_attr(SEXP R_group,;
extern SEXP R_dump(SEXP R_adios_path,;
extern SEXP R_read(SEXP R_adios_path,;
extern SEXP R_stage_read(SEXP R_adios_file_ptr,;
extern SEXP R_write(SEXP R_filename,;
extern SEXP copy_read (SEXP R_adios_var_info, ;
extern SEXP dump_vars (SEXP R_adios_fp);
extern SEXP readVar(SEXP R_adios_fp, ;

static const R_CallMethodDef CallEntries[] = {
  {"R_adios_advance_step", (DL_FUNC) &R_adios_advance_step, 1},
  {"R_adios_allocate_buffer", (DL_FUNC) &R_adios_allocate_buffer, 1},
  {"R_adios_attr_read", (DL_FUNC) &R_adios_attr_read, 1},
  {"R_adios_close", (DL_FUNC) &R_adios_close, 1},
  {"R_adios_declare_group", (DL_FUNC) &R_adios_declare_group, 1},
  {"R_adios_define_var", (DL_FUNC) &R_adios_define_var, 1},
  {"R_adios_errno", (DL_FUNC) &R_adios_errno, 0},
  {"R_adios_finalize", (DL_FUNC) &R_adios_finalize, 1},
  {"R_adios_free_varinfo ", (DL_FUNC) &R_adios_free_varinfo , 1},
  {"R_adios_get_attr", (DL_FUNC) &R_adios_get_attr, 1},
  {"R_adios_get_attr_byid", (DL_FUNC) &R_adios_get_attr_byid, 1},
  {"R_adios_group_size", (DL_FUNC) &R_adios_group_size, 1},
  {"R_adios_init_noxml", (DL_FUNC) &R_adios_init_noxml, 1},
  {"R_adios_inq_var", (DL_FUNC) &R_adios_inq_var, 1},
  {"R_adios_inq_var_blockinfo", (DL_FUNC) &R_adios_inq_var_blockinfo, 1},
  {"R_adios_inq_var_byid", (DL_FUNC) &R_adios_inq_var_byid, 1},
  {"R_adios_open", (DL_FUNC) &R_adios_open, 1},
  {"R_adios_perform_reads", (DL_FUNC) &R_adios_perform_reads, 1},
  {"R_adios_print_attr", (DL_FUNC) &R_adios_print_attr, 1},
  {"R_adios_read_close", (DL_FUNC) &R_adios_read_close, 1},
  {"R_adios_read_finalize_method", (DL_FUNC) &R_adios_read_finalize_method, 1},
  {"R_adios_read_init_method", (DL_FUNC) &R_adios_read_init_method, 1},
  {"R_adios_read_open", (DL_FUNC) &R_adios_read_open, 1},
  {"R_adios_release_step", (DL_FUNC) &R_adios_release_step, 1},
  {"R_adios_schedule_read", (DL_FUNC) &R_adios_schedule_read, 1},
  {"R_adios_select_method", (DL_FUNC) &R_adios_select_method, 1},
  {"R_adios_selection_boundingbox", (DL_FUNC) &R_adios_selection_boundingbox, 1},
  {"R_adios_set_max_buffer_size", (DL_FUNC) &R_adios_set_max_buffer_size, 1},
  {"R_adios_var_attr_read", (DL_FUNC) &R_adios_var_attr_read, 1},
  {"R_adios_write", (DL_FUNC) &R_adios_write, 1},
  {"R_append", (DL_FUNC) &R_append, 1},
  {"R_bpls", (DL_FUNC) &R_bpls, 1},
  {"R_create", (DL_FUNC) &R_create, 1},
  {"R_custom_data_access", (DL_FUNC) &R_custom_data_access, 1},
  {"R_custom_inq_var_dims", (DL_FUNC) &R_custom_inq_var_dims, 1},
  {"R_custom_inq_var_ndim", (DL_FUNC) &R_custom_inq_var_ndim, 1},
  {"R_define_attr", (DL_FUNC) &R_define_attr, 1},
  {"R_dump", (DL_FUNC) &R_dump, 1},
  {"R_read", (DL_FUNC) &R_read, 1},
  {"R_stage_read", (DL_FUNC) &R_stage_read, 1},
  {"R_write", (DL_FUNC) &R_write, 1},
  {"copy_read ", (DL_FUNC) &copy_read , 1},
  {"dump_vars ", (DL_FUNC) &dump_vars , 1},
  {"readVar", (DL_FUNC) &readVar, 1},
  {NULL, NULL, 0}
};

void R_init_pbdADIOS(DllInfo *dll)
{
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}
