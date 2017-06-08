#' Utilities to read from ADIOS, including staging.
#'
#' @name pbdADIOS
#'
#' @useDynLib pbdADIOS,
#   ### Read API
#'  R_adios_read_init_method,
#'  R_adios_read_open,
#'  R_adios_inq_var,
#'  R_adios_inq_var_byid,
#'  R_adios_free_varinfo,
#'  R_adios_inq_var_blockinfo,
#'  R_custom_inq_var_ndim,
#'  R_custom_inq_var_dims,
#'  R_adios_selection_boundingbox,
#'  R_adios_schedule_read,
#'  R_adios_perform_reads,
#'  R_custom_data_access,
#'  R_adios_advance_step,
#'  R_adios_read_close,
#'  R_adios_read_finalize_method,
#'  R_adios_errno,
#'
#   ### Write API
#'  R_adios_init_noxml,
#'  R_adios_set_max_buffer_size,
#'  R_adios_declare_group,
#'  R_adios_select_method,
#'  R_adios_define_var,
#'  R_adios_open,
#'  R_adios_group_size,
#'  R_adios_write,
#'  R_adios_close,
#'  R_adios_finalize,
#'
#   ### ADIOS get attributes API
#'  R_adios_get_attr,
#'  R_adios_get_attr_byid,
#'  R_adios_print_attr,
#'  R_adios_attr_read,
#'  R_adios_var_attr_read,
#'
#   ### ADIOS higher level function
#'  R_bpls,
#'  R_dump,
#'  R_write,
#'  R_append,
#'  R_read,
#'  R_stage_read
#'
#'
#' @import pbdMPI
#' @docType package
#' @title pbdADIOS: an R wrapper for ADIOS
#' @author The pbdR Core Team
#' @keywords pbdADIOS
NULL
