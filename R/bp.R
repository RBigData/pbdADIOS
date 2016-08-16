
#' @title bpls
#'
#' @description 
#' This function is used to print variables and attributes.
#' 
#' @param adios.filename adios file name
#' 
#' @examples
#' bpls("attributes.bp")
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

#' @title bp.dump
#'
#' @description 
#' This function is used to dump all variables
#'
#' @param adios.filename
#'
#' @return a list of variables
#'
#' @examples
#' bp.dump("attributes.bp")
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

#' @title bp.read
#' 
#' @description 
#' This function is used to read variables. You can read one or more variables 
#' with specific range or whole.
#' 
#' @param adios.filename adios file name
#' @param varname 
#' variable name
#'
#' @return variable values. If start and count are not specified, all values will be returned.
#'
#' @examples
#' read a scalar
#' bp.read("attributes.bp", "NX")
#'
#' read a variable with start
#' bp.read("attributes.bp", "temperature", c(0, 0))
#'
#' read a variable with count
#' bp.read("attributes.bp", "temperature", count=c(1, 2))
#'
#' read a variable with start and count
#' bp.read("attributes.bp", "temperature", c(0, 9), c(1, 1))
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

#' @title bp.create
#' 
#' @description 
#' This function is create adios group and store file name.
#' 
#' @param adios.filename adios file name
#' @param adios.groupname adios group name
#' @param comm
#'
#' @return group pointer
#'
#' @examples
#' bp.create("bptest.bp")
#' 
#' @export
bp.create <- function(adios.filename,
                      adios.groupname = "bp.group",
                      buffer.size = 20,
                      comm = .pbd_env$SPMD.CT$comm)
{
    # Init ADIOS environment
    init_state()

    .adiosenv$adios.filename <- adios.filename
    .adiosenv$adios.groupname <- adios.groupname
    .adiosenv$adios.group <- as.numeric(.Call("R_create", 
                                        as.character(adios.groupname), 
                                        as.integer(buffer.size),
                                        comm.c2f(comm)))

    invisible()
}

#' @title bp.var
#' 
#' @description 
#' This function is used to register variable names and values.
#' 
#' @param adios.varname variable name
#' @param data 
#'
#' @examples
#' bp.var("a", a)
#' 
#' @export
bp.var <- function(adios.varname, data)
{
    .adiosenv$nvars <- .adiosenv$nvars + 1
    .adiosenv$varname_list[[.adiosenv$nvars]] <- as.character(adios.varname)
    # Check if data is double
    if(is.double(data)) {
        .adiosenv$adios.type[[.adiosenv$nvars]] <- as.integer(1)
    }else {
        .adiosenv$adios.type[[.adiosenv$nvars]] <- as.integer(0)
    }
    # Calculate the dim of data
    if(is.vector(data)) {
        .adiosenv$var_list[[.adiosenv$nvars]] <- data
        .adiosenv$varlength_list[[.adiosenv$nvars]] <- length(data)
        .adiosenv$ndim[[.adiosenv$nvars]] <- as.integer(1)
    }else {
        .adiosenv$var_list[[.adiosenv$nvars]] <- data
        .adiosenv$varlength_list[[.adiosenv$nvars]] <- rev(dim(data))
        .adiosenv$ndim[[.adiosenv$nvars]] <- length(dim(data))
    }

    invisible()
}

#' @title bp.write
#' 
#' @description 
#' This function is used to write or append variables to bp file.
#'
#' @examples
#' bp.write()
#' 
#' @export
bp.write <- function(comm = .pbd_env$SPMD.CT$comm,
                     p = comm.size(.pbd_env$SPMD.CT$comm),
                     adios.rank = comm.rank(.pbd_env$SPMD.CT$comm))
{
    if(.adiosenv$adios.tag == 0) {
        .Call("R_write", 
              as.character(.adiosenv$adios.filename),
              .adiosenv$adios.group,
              as.character(.adiosenv$adios.groupname),
              as.integer(.adiosenv$nvars),
              .adiosenv$varname_list,
              .adiosenv$var_list,
              .adiosenv$varlength_list,
              .adiosenv$ndim,
              .adiosenv$adios.type,
              comm.c2f(comm),
              as.integer(comm.size(.pbd_env$SPMD.CT$comm)),
              as.integer(adios.rank))
        .adiosenv$adios.tag <- 1
    }else {
        .Call("R_append", 
              as.character(.adiosenv$adios.filename),
              .adiosenv$adios.group,
              as.character(.adiosenv$adios.groupname),
              as.integer(.adiosenv$nvars),
              .adiosenv$varname_list,
              .adiosenv$var_list,
              .adiosenv$varlength_list,
              .adiosenv$ndim,
              .adiosenv$adios.type,
              comm.c2f(comm),
              as.integer(p),
              as.integer(adios.rank))
    }

    #invisible()
}

#' @title bp.attr
#' 
#' @description 
#' This function is used to define attributes for a bp file.
#' 
#' @param adios.attrname adios attribute name
#' @param data
#'
#' @examples
#' today <- Sys.Date()
#' bp.attr("date", format(today, format="%B %d %Y"))
#' bp.attr("README", "This is just a test.")
#' 
#' @export
bp.attr <- function(adios.attrname, data)
{
    .Call("R_define_attr",
          .adiosenv$adios.group,
          as.character(adios.attrname),
          as.integer(length(data)),
          data)
    
    invisible()
}

#' @title bp.flush
#' 
#' @description 
#' This function is used to close adios and clear custom environment
#'
#' @examples
#' bp.flush()
#' 
#' @export
bp.flush <- function()
{
    adios.finalize()
    # Clear custom environment
    rm(list = ls(pos = .adiosenv), pos = .adiosenv)
    
    invisible()
}





