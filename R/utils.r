#' adios.errno
#' 
#' Get the ADIOS errno.
#' 
#' @return
#' The errno.
#' 
#' @export
adios.errno <- function()
{
  .Call(R_adios_errno)
}


#' adios.finalize
#' 
#' Shut down ADIOS.
#' 
#' @param comm
#' The MPI communicator.
#' 
#' @return
#' \code{NULL}
#' 
#' @export
adios.finalize <- function(comm = pbdMPI::.SPMD.CT$comm)
{
  .Call(R_adios_finalize, comm.rank(comm))
  invisible()
}
