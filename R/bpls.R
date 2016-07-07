#' @title print variables and attributes
#' 
#' @param adios.filename
#' @param comm
#' @param adios.rank
#'
#' @export
bpls <- function(adios.filename,
                 comm = .pbd_env$SPMD.CT$comm,
                 adios.rank = comm.rank(.pbd_env$SPMD.CT$comm))
{
    .Call("R_bpls", 
          as.character(adios.filename),
          comm.c2f(comm),
          as.integer(adios.rank))
    invisible()
}

#' @title dump variables
#'
#' @param adios.filename
#' @param comm
#' @param adios.rank
#'
#' @return a list of variables
#'
#' @export
bp.dump <- function(adios.filename,
                 comm = .pbd_env$SPMD.CT$comm,
                 adios.rank = comm.rank(.pbd_env$SPMD.CT$comm))
{
    .Call("R_dump", 
          as.character(adios.filename),
          comm.c2f(comm),
          as.integer(adios.rank))
}

#' @title read variable
#' 
#' @param adios.filename
#' @param varname
#' @param comm
#' @param adios.rank
#'
#' @return variable values. If start and count are not specified, all values will be returned.
#'
#' @export
bp.read <- function(adios.filename,
                    varname,
                    comm = .pbd_env$SPMD.CT$comm,
                    adios.rank = comm.rank(.pbd_env$SPMD.CT$comm))
{
    .Call("R_read", 
          as.character(adios.filename),
          as.character(varname),
          comm.c2f(comm),
          as.integer(adios.rank))
}
