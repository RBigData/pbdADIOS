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
                    p = comm.size(.pbd_env$SPMD.CT$comm),
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

        start = as.list(start)
        count = as.list(count)

    }else {
        if(is.null(start))
            start = -1
        if(is.null(count))
            count = -2

        start = list(start)
        count = list(count)
    }

    # convert start and count to integer list
    for (i in 1:nvars){
        start[[i]] = as.integer(start[[i]])
        count[[i]] = as.integer(count[[i]])
    }

    .Call("R_read", 
          as.character(adios.filename),
          as.list(varname),
          start,
          count,
          as.integer(nvars),
          comm.c2f(comm),
          as.integer(p),
          as.integer(adios.rank))
}

#' @title create adios group and store file name
#' 
#' @param adios.filename
#' @param adios.groupname
#' @param comm
#'
#' @return group pointer. 
#'
#' @export
bp.create <- function(adios.filename,
                      adios.groupname = "bp.group",
                      buffer.size = 20,
                      comm = .pbd_env$SPMD.CT$comm)
{
    # Assign related variables to global environment
    adios.filename <<- adios.filename
    adios.groupname <<- adios.groupname

    nvars <<- 0
    varname_list <<- list()
    var_list <<- list()
    varlength_list <<- list()
    ndim <<- list()
    # adios.tag = 0, write; adios.tag = 1, append
    adios.tag <<- 0
    # adios.type = 0, int; adios.type = 1, double
    adios.type <<- list()

    adios.group <<- as.numeric(.Call("R_create", 
                                     as.character(adios.groupname), 
                                     as.integer(buffer.size),
                                     comm.c2f(comm)))

    invisible()
}

#' @title register variable names and values
#' 
#' @param adios.varname
#' @param data
#'
#' @export
bp.var <- function(adios.varname, data)
{
    nvars <<- nvars + 1
    varname_list[[nvars]] <<- as.character(adios.varname)
    # Check if data is double
    if(is.double(data)) {
        adios.type[[nvars]] <<- as.integer(1)
    }else {
        adios.type[[nvars]] <<- as.integer(0)
    }
    # Calculate the dim of data
    if(is.vector(data)) {
        var_list[[nvars]] <<- data
        varlength_list[[nvars]] <<- length(data)
        ndim[[nvars]] <<- as.integer(1)
    }else {
        var_list[[nvars]] <<- data
        varlength_list[[nvars]] <<- rev(dim(data))
        ndim[[nvars]] <<- length(dim(data))
    }

    invisible()
}

#' @title write or append variables to bp file
#' 
#' @param comm
#' @param p
#' @param adios.rank
#'
#' @export
bp.write <- function(comm = .pbd_env$SPMD.CT$comm,
                     p = comm.size(.pbd_env$SPMD.CT$comm),
                     adios.rank = comm.rank(.pbd_env$SPMD.CT$comm))
{
    if(adios.tag == 0) {
        .Call("R_write", 
              as.character(adios.filename),
              adios.group,
              as.character(adios.groupname),
              as.integer(nvars),
              varname_list,
              var_list,
              varlength_list,
              ndim,
              adios.type,
              comm.c2f(comm),
              as.integer(comm.size(.pbd_env$SPMD.CT$comm)),
              as.integer(adios.rank))
        adios.tag <<- 1
    }else {
        .Call("R_append", 
              as.character(adios.filename),
              adios.group,
              as.character(adios.groupname),
              as.integer(nvars),
              varname_list,
              var_list,
              varlength_list,
              ndim,
              adios.type,
              comm.c2f(comm),
              as.integer(p),
              as.integer(adios.rank))
    }

    #invisible()
}

#' @title define attributes
#' 
#' @param adios.attrname
#' @param data
#'
#' @export
bp.attr <- function(adios.attrname, data)
{
    .Call("R_define_attr",
          adios.group,
          as.character(adios.attrname),
          as.integer(length(data)),
          data)
    
    invisible()
}

#' @title close adios and clear global environment
#'
#' @export
bp.flush <- function()
{
    adios.finalize()
    # Clear global environment
    rm(list = ls(pos = ".GlobalEnv"), pos = ".GlobalEnv")
    
    invisible()
}





