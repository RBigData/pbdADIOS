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
                    start = -1,
                    count = -2,
                    comm = .pbd_env$SPMD.CT$comm,
                    adios.rank = comm.rank(.pbd_env$SPMD.CT$comm))
{
    .Call("R_read", 
          as.character(adios.filename),
          as.character(varname),
          as.integer(start),
          as.integer(count),
          comm.c2f(comm),
          as.integer(adios.rank))
}


bp.read <- function(adios.filename,
                    varname,
                    start = -1,
                    count = -2,
                    comm = .pbd_env$SPMD.CT$comm,
                    adios.rank = comm.rank(.pbd_env$SPMD.CT$comm))
{ 
  # check if nvars is null
  if(is.null(nvars)) {
    cat("The varname can't be empty")
  }else {
    # calculate the number of vars
    nvars = length(varname)
   
    # check the length of start
    if((start != -1) && (length(start) != l)) {
      cat("The length of start is not correct")
    }
    # check the length of count
    else if((count != -1) && (length(start) != l)){
      cat("The length of count is not correct")
    }
    # If the length is greater than 1, the parameters should be lists.
    else if(nvars > 1) {
      if((start != -1) && !is.list(start)) {
        cat("The start should be a list")
      }else if((count != -2) && !is.list(count)) {
        cat("The count should be a list")
      }else {
        #call
        .Call("R_read", 
          as.character(adios.filename),
          as.list(varname),
          as.list(start),
          as.list(count),
          as.integer(nvars),
          comm.c2f(comm),
          as.integer(adios.rank))
      }
    }else {
      #call
        .Call("R_read", 
          as.character(adios.filename),
          as.list(varname),
          as.list(start),
          as.list(count),
          as.integer(nvars),
          comm.c2f(comm),
          as.integer(adios.rank))
    }
  }
}

