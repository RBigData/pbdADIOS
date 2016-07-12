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
                    start = NULL,
                    count = NULL,
                    comm = .pbd_env$SPMD.CT$comm,
                    adios.rank = comm.rank(.pbd_env$SPMD.CT$comm))
{ 
    # check if varnmae is null
    if(is.null(varname))
        stop("The varname can't be empty!")

    # calculate the number of vars
    nvars = length(varname)
    
    if(nvars != 1) {
        # check the length of start
        if(!is.null(start)) {
            if(!is.list(start))
               stop("Start should be a list!")

            if(length(start) != nvars)
                stop("The length of start is not correct!")    
        }else {
            start = rep(-1, nvars)
        }

        # check the length of count
        if(!is.null(count)) {
            if(!is.list(count))
               stop("Count should be a list!")

            if(length(count) != nvars)
                stop("The length of count is not correct!")    
        }else {
            count = rep(-2, nvars)
        }
    }else {
        if(is.null(start))
            start = -1;
        if(is.null(count))
            count = -2
    }

    .Call("R_read", 
          as.character(adios.filename),
          as.list(varname),
          as.list(start),
          as.list(count),
          as.integer(nvars),
          comm.c2f(comm),
          as.integer(adios.rank))
}

