#' @title Print adios attributes
#' 
#' @description
#' This function is used to print all the attributes in a bp file.
#'
#' @param adios.rank comm rank
#' @param adios.fp adios file pointer
#'
adios.attr.read <- function(adios.rank = comm.rank(.pbd_env$SPMD.CT$comm), 
                            adios.fp)
{
    .Call("R_adios_attr_read", 
          as.integer(adios.rank),
          adios.fp)
    invisible()
}

#' @title Print adios variables and attributes
#' 
#' @description
#' This function is used to print all adios variables with related attributes in a bp file.
#'
#' @param adios.rank comm rank
#' @param adios.fp adios file pointer
#'
adios.var.attr.read <- function(adios.rank = comm.rank(.pbd_env$SPMD.CT$comm), 
                                adios.fp)
{
    .Call("R_adios_var_attr_read", 
          as.integer(adios.rank),
          adios.fp)
    invisible()
}