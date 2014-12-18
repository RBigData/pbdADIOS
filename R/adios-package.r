#'  Package to read from ADIOS, including staging.
#'
#' @name pbdADIOS-package
#' @useDynLib pbdADIOS,
#'   R_adios_read_init_method, 
#'   R_adios_read_open, 
#'   R_adios_read_close, 
#'   R_adios_read_finalize_method, 
#'   R_adios_inq_var, 
#'   R_custom_inq_var_ndim,
#'   R_custom_inq_var_dims, 
#'   R_adios_inq_var_blockinfo, 
#'   R_adios_selection_bounding_box, 
#'   R_adios_schedule_read,
#'   R_custom_data_access,
#'   R_adios_perform_reads,
#'   R_adios_advance_step,
#'   R_adios_errno,
#'   R_adios_open,
#'   R_adios_close,
#'   R_adios_group_size,
#'   R_adios_init,
#'   R_adios_finalize,
#'   R_adios_write
#'   
#' @import pbdMPI
#' @docType package
#' @title logisticPCA-package
#' @author Pragneshkumar Patel, George Ostrouchov, Wei-Chen Chen, Drew Schmidt
#' @keywords package
NULL
