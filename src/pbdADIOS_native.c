/* Automatically generated. Do not edit by hand. */

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <stdlib.h>

extern SEXP R_adios_advance_step(SEXP R_adios_file_ptr, SEXP R_adios_last, SEXP R_adios_timeout_sec);
extern SEXP R_adios_allocate_buffer(SEXP R_adios_buffer_when, SEXP R_buffer_size);
extern SEXP R_adios_attr_read(SEXP R_adios_rank, SEXP R_adios_fp);
extern SEXP R_adios_close(SEXP R_m_adios_file);
extern SEXP R_adios_declare_group(SEXP R_adios_group_name, SEXP R_adios_time_index, SEXP R_adios_flag);
extern SEXP R_adios_define_var(SEXP R_m_adios_group, SEXP R_adios_varname, SEXP R_adios_path, SEXP R_adios_type, SEXP R_adios_local_dim, SEXP R_adios_global_dim, SEXP R_adios_local_offset);
extern SEXP R_adios_errno();
extern SEXP R_adios_finalize(SEXP R_comm_rank);
extern SEXP R_adios_free_varinfo(SEXP R_adios_var_info);
extern SEXP R_adios_get_attr(SEXP R_adios_fp, SEXP R_adios_attrname, SEXP R_adios_getattr);
extern SEXP R_adios_get_attr_byid(SEXP R_adios_fp, SEXP R_adios_attrid, SEXP R_adios_getattr);
extern SEXP R_adios_group_size(SEXP R_m_adios_file, SEXP R_adios_group_size);
extern SEXP R_adios_init_noxml(SEXP R_comm);
extern SEXP R_adios_inq_var(SEXP R_adios_file_ptr, SEXP R_adios_varname);
extern SEXP R_adios_inq_var_blockinfo(SEXP R_adios_file_ptr, SEXP R_adios_var_info);
extern SEXP R_adios_inq_var_byid(SEXP R_adios_file_ptr, SEXP R_adios_varid);
extern SEXP R_adios_open(SEXP R_adios_group_name, SEXP R_adios_file_name, SEXP R_adios_mode, SEXP R_comm);
extern SEXP R_adios_perform_reads(SEXP R_adios_file_ptr, SEXP R_adios_blocking);
extern SEXP R_adios_print_attr(SEXP R_adios_rank, SEXP R_adios_fp, SEXP R_adios_attrid, SEXP R_adios_get_attr);
extern SEXP R_adios_read_close(SEXP R_adios_file_ptr);
extern SEXP R_adios_read_finalize_method(SEXP R_adios_read_method);
extern SEXP R_adios_read_init_method(SEXP R_adios_read_method, SEXP R_comm, SEXP R_params);
extern SEXP R_adios_read_open(SEXP R_filename, SEXP R_adios_read_method, SEXP R_comm, SEXP R_adios_lockmode, SEXP R_timeout_sec);
extern SEXP R_adios_release_step(SEXP R_adios_file_ptr);
extern SEXP R_adios_schedule_read(SEXP R_adios_var_info, SEXP R_adios_start, SEXP R_adios_count, SEXP R_adios_file_ptr, SEXP R_adios_selection, SEXP R_adios_varname, SEXP R_adios_from_steps, SEXP R_adios_nsteps);
extern SEXP R_adios_select_method(SEXP R_m_adios_group, SEXP R_adios_method, SEXP R_adios_params, SEXP R_adios_base_path );
extern SEXP R_adios_selection_boundingbox(SEXP R_adios_ndim, SEXP R_adios_start, SEXP R_adios_count);
extern SEXP R_adios_set_max_buffer_size(SEXP R_max_buffer_size_MB);
extern SEXP R_adios_var_attr_read(SEXP R_adios_rank, SEXP R_adios_fp);
extern SEXP R_adios_write(SEXP R_m_adios_file, SEXP R_adios_var_name, SEXP R_adios_var);
extern SEXP R_append(SEXP R_filename, SEXP R_group, SEXP R_groupname, SEXP R_nvars, SEXP R_varname_list, SEXP R_var_list, SEXP R_varlength_list, SEXP R_ndim, SEXP R_type, SEXP R_comm, SEXP R_p, SEXP R_adios_rank);
extern SEXP R_bpls(SEXP R_adios_path, SEXP R_comm, SEXP R_adios_rank);
extern SEXP R_create(SEXP R_groupname, SEXP R_buffersize, SEXP R_comm);
extern SEXP R_custom_data_access(SEXP R_adios_data, SEXP R_adios_selection, SEXP R_adios_var_info);
extern SEXP R_custom_inq_var_dims(SEXP R_adios_var_info);
extern SEXP R_custom_inq_var_ndim(SEXP R_adios_var_info);
extern SEXP R_define_attr(SEXP R_group, SEXP R_attrname, SEXP R_nelems, SEXP R_values);
extern SEXP R_dump(SEXP R_adios_path, SEXP R_comm, SEXP R_adios_rank);
extern SEXP R_read(SEXP R_adios_path, SEXP R_varname, SEXP R_start, SEXP R_count, SEXP R_nvars, SEXP R_comm, SEXP R_p, SEXP R_adios_rank);
extern SEXP R_stage_read(SEXP R_adios_file_ptr, SEXP R_varname, SEXP R_nvars, SEXP R_comm, SEXP R_p, SEXP R_adios_rank);
extern SEXP R_write(SEXP R_filename, SEXP R_group, SEXP R_groupname, SEXP R_nvars, SEXP R_varname_list, SEXP R_var_list, SEXP R_varlength_list, SEXP R_ndim, SEXP R_type, SEXP R_comm, SEXP R_p, SEXP R_adios_rank);
extern SEXP copy_read (SEXP R_adios_var_info, SEXP R_nelems, SEXP R_data);
extern SEXP dump_vars (SEXP R_adios_fp);
extern SEXP readVar(SEXP R_adios_fp, SEXP R_adios_var_info, SEXP R_name, SEXP R_timed);

static const R_CallMethodDef CallEntries[] = {
  {"R_adios_advance_step", (DL_FUNC) &R_adios_advance_step, 3},
  {"R_adios_allocate_buffer", (DL_FUNC) &R_adios_allocate_buffer, 2},
  {"R_adios_attr_read", (DL_FUNC) &R_adios_attr_read, 2},
  {"R_adios_close", (DL_FUNC) &R_adios_close, 1},
  {"R_adios_declare_group", (DL_FUNC) &R_adios_declare_group, 3},
  {"R_adios_define_var", (DL_FUNC) &R_adios_define_var, 7},
  {"R_adios_errno", (DL_FUNC) &R_adios_errno, 0},
  {"R_adios_finalize", (DL_FUNC) &R_adios_finalize, 1},
  {"R_adios_free_varinfo", (DL_FUNC) &R_adios_free_varinfo, 1},
  {"R_adios_get_attr", (DL_FUNC) &R_adios_get_attr, 3},
  {"R_adios_get_attr_byid", (DL_FUNC) &R_adios_get_attr_byid, 3},
  {"R_adios_group_size", (DL_FUNC) &R_adios_group_size, 2},
  {"R_adios_init_noxml", (DL_FUNC) &R_adios_init_noxml, 1},
  {"R_adios_inq_var", (DL_FUNC) &R_adios_inq_var, 2},
  {"R_adios_inq_var_blockinfo", (DL_FUNC) &R_adios_inq_var_blockinfo, 2},
  {"R_adios_inq_var_byid", (DL_FUNC) &R_adios_inq_var_byid, 2},
  {"R_adios_open", (DL_FUNC) &R_adios_open, 4},
  {"R_adios_perform_reads", (DL_FUNC) &R_adios_perform_reads, 2},
  {"R_adios_print_attr", (DL_FUNC) &R_adios_print_attr, 4},
  {"R_adios_read_close", (DL_FUNC) &R_adios_read_close, 1},
  {"R_adios_read_finalize_method", (DL_FUNC) &R_adios_read_finalize_method, 1},
  {"R_adios_read_init_method", (DL_FUNC) &R_adios_read_init_method, 3},
  {"R_adios_read_open", (DL_FUNC) &R_adios_read_open, 5},
  {"R_adios_release_step", (DL_FUNC) &R_adios_release_step, 1},
  {"R_adios_schedule_read", (DL_FUNC) &R_adios_schedule_read, 8},
  {"R_adios_select_method", (DL_FUNC) &R_adios_select_method, 4},
  {"R_adios_selection_boundingbox", (DL_FUNC) &R_adios_selection_boundingbox, 3},
  {"R_adios_set_max_buffer_size", (DL_FUNC) &R_adios_set_max_buffer_size, 1},
  {"R_adios_var_attr_read", (DL_FUNC) &R_adios_var_attr_read, 2},
  {"R_adios_write", (DL_FUNC) &R_adios_write, 3},
  {"R_append", (DL_FUNC) &R_append, 12},
  {"R_bpls", (DL_FUNC) &R_bpls, 3},
  {"R_create", (DL_FUNC) &R_create, 3},
  {"R_custom_data_access", (DL_FUNC) &R_custom_data_access, 3},
  {"R_custom_inq_var_dims", (DL_FUNC) &R_custom_inq_var_dims, 1},
  {"R_custom_inq_var_ndim", (DL_FUNC) &R_custom_inq_var_ndim, 1},
  {"R_define_attr", (DL_FUNC) &R_define_attr, 4},
  {"R_dump", (DL_FUNC) &R_dump, 3},
  {"R_read", (DL_FUNC) &R_read, 8},
  {"R_stage_read", (DL_FUNC) &R_stage_read, 6},
  {"R_write", (DL_FUNC) &R_write, 12},
  {"copy_read ", (DL_FUNC) &copy_read , 3},
  {"dump_vars ", (DL_FUNC) &dump_vars , 1},
  {"readVar", (DL_FUNC) &readVar, 4},
  {NULL, NULL, 0}
};
void R_init_pbdADIOS(DllInfo *dll)
{
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}
