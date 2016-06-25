#' @title print all adios attributes
#' 
#' @param adios.rank comm rank
#' @param adios.fp
#'
#' @export
adios.attr.read <- function(adios.rank = comm.rank(.pbd_env$SPMD.CT$comm), 
                            adios.fp)
{
    .Call("R_adios_attr_read", 
          as.integer(adios.rank),
          adios.fp)
}
