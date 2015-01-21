#'  Package to read from ADIOS, including staging.
#'
#' @name pbdADIOS-package
#'
#' @exportPattern "^adios\\.[[:alpha:]]+"
#' @exportPattern "^custom\\.[[:alpha:]]+"
#'
#' @useDynLib pbdADIOS,
#   ### Reader
#'  R_adios_read_init_method,
#'  R_adios_read_open,
#'  R_adios_read_close,
#'  R_adios_read_finalize_method,
#'  R_adios_inq_var,
#'  R_custom_inq_var_ndim,
#'  R_custom_inq_var_dims,
#'  R_adios_inq_var_blockinfo,
#'  R_adios_selection_bounding_box,
#'  R_adios_schedule_read,
#'  R_custom_data_access,
#'  R_adios_perform_reads,
#'  R_adios_advance_step,
#'  R_adios_errno,
#   ### Writer
#'  R_adios_init_noxml,
#'  R_adios_allocate_buffer,
#'  R_adios_declare_group,
#'  R_adios_select_method,
#'  R_adios_define_var,
#'  R_adios_open,
#'  R_adios_group_size,
#'  R_adios_write,
#'  R_adios_close,
#'  R_adios_finalize
#'
#' @import pbdMPI
#' @docType package
#' @title logisticPCA-package
#' @author Pragneshkumar Patel, George Ostrouchov, Wei-Chen Chen, Drew Schmidt
#' @keywords package
NULL
