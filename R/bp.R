
#' @title bpls
#'
#' @description 
#' This function is used to print variables and attributes.
#' 
#' @param adios.filename adios file name
#' @param comm mpi comm
#' @param adios.rank comm rank
#' 
#' @examples
#' \dontrun{
#' bpls("attributes.bp")
#' }
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
#' @param adios.filename adios file name
#' @param comm mpi comm
#' @param adios.rank comm rank
#'
#' @return a list of variables
#'
#' @examples
#' \dontrun{
#' bp.dump("attributes.bp")
#' }
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
#' @param varname variable name
#' @param start start position of selection
#' @param count length of selection
#' @param comm mpi comm
#' @param p number of processes
#' @param adios.rank comm rank
#'
#' @return variable values. If start and count are not specified, all values will be returned.
#'
#' @examples
#' \dontrun{
#' ##read a scalar
#' bp.read("attributes.bp", "NX")
#'
#' ##read a variable with start
#' bp.read("attributes.bp", "temperature", c(0, 0))
#'
#' ##read a variable with count
#' bp.read("attributes.bp", "temperature", count=c(1, 2))
#'
#' ##read a variable with start and count
#' bp.read("attributes.bp", "temperature", c(0, 9), c(1, 1))
#' }
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
#' @param buffer.size buffer size for write stream
#' @param comm mpi comm
#'
#' @return group pointer
#'
#' @examples
#' \dontrun{
#' bp.create("bptest.bp")
#' }
#' 
#' @export
bp.create <- function(adios.filename,
                      adios.groupname = "bp.group",
                      buffer.size = 20,
                      comm = .pbd_env$SPMD.CT$comm)
{
    # Init ADIOS environment
    init_state()

    .GlobalEnv$.adiosenv$adios.filename <- adios.filename
    .GlobalEnv$.adiosenv$adios.groupname <- adios.groupname
    .GlobalEnv$.adiosenv$adios.group <- as.numeric(.Call("R_create", 
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
#' @param data value of the variable
#' 
#' @export
bp.var <- function(adios.varname, data)
{
    .GlobalEnv$.adiosenv$nvars <- .GlobalEnv$.adiosenv$nvars + 1
    .GlobalEnv$.adiosenv$varname_list[[.GlobalEnv$.adiosenv$nvars]] <- as.character(adios.varname)
    # Check if data is double
    if(is.double(data)) {
        .GlobalEnv$.adiosenv$adios.type[[.GlobalEnv$.adiosenv$nvars]] <- as.integer(1)
    }else {
        .GlobalEnv$.adiosenv$adios.type[[.GlobalEnv$.adiosenv$nvars]] <- as.integer(0)
    }
    # Calculate the dim of data
    if(is.vector(data)) {
        .GlobalEnv$.adiosenv$var_list[[.GlobalEnv$.adiosenv$nvars]] <- data
        .GlobalEnv$.adiosenv$varlength_list[[.GlobalEnv$.adiosenv$nvars]] <- length(data)
        .GlobalEnv$.adiosenv$ndim[[.GlobalEnv$.adiosenv$nvars]] <- as.integer(1)
    }else {
        .GlobalEnv$.adiosenv$var_list[[.GlobalEnv$.adiosenv$nvars]] <- data
        .GlobalEnv$.adiosenv$varlength_list[[.GlobalEnv$.adiosenv$nvars]] <- rev(dim(data))
        .GlobalEnv$.adiosenv$ndim[[.GlobalEnv$.adiosenv$nvars]] <- length(dim(data))
    }

    invisible()
}

#' @title bp.write
#' 
#' @description 
#' This function is used to write or append variables to bp file.
#'
#' @param comm mpi comm
#' @param p number of processes
#' @param adios.rank comm rank
#' 
#' @export
bp.write <- function(comm = .pbd_env$SPMD.CT$comm,
                     p = comm.size(.pbd_env$SPMD.CT$comm),
                     adios.rank = comm.rank(.pbd_env$SPMD.CT$comm))
{
    if(.GlobalEnv$.adiosenv$adios.tag == 0) {
        .Call("R_write", 
              as.character(.GlobalEnv$.adiosenv$adios.filename),
              .GlobalEnv$.adiosenv$adios.group,
              as.character(.GlobalEnv$.adiosenv$adios.groupname),
              as.integer(.GlobalEnv$.adiosenv$nvars),
              .GlobalEnv$.adiosenv$varname_list,
              .GlobalEnv$.adiosenv$var_list,
              .GlobalEnv$.adiosenv$varlength_list,
              .GlobalEnv$.adiosenv$ndim,
              .GlobalEnv$.adiosenv$adios.type,
              comm.c2f(comm),
              as.integer(comm.size(.pbd_env$SPMD.CT$comm)),
              as.integer(adios.rank))
        .GlobalEnv$.adiosenv$adios.tag <- 1
    }else {
        .Call("R_append", 
              as.character(.GlobalEnv$.adiosenv$adios.filename),
              .GlobalEnv$.adiosenv$adios.group,
              as.character(.GlobalEnv$.adiosenv$adios.groupname),
              as.integer(.GlobalEnv$.adiosenv$nvars),
              .GlobalEnv$.adiosenv$varname_list,
              .GlobalEnv$.adiosenv$var_list,
              .GlobalEnv$.adiosenv$varlength_list,
              .GlobalEnv$.adiosenv$ndim,
              .GlobalEnv$.adiosenv$adios.type,
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
#' @param data value of the attribute
#'
#' @examples
#' \dontrun{
#' today <- Sys.Date()
#' bp.attr("date", format(today, format="%B %d %Y"))
#' bp.attr("README", "This is just a test.")
#' }
#' 
#' @export
bp.attr <- function(adios.attrname, data)
{
    if (!is.null(.GlobalEnv$.adiosenv$adios.group)) { 
        .Call("R_define_attr",
              .GlobalEnv$.adiosenv$adios.group,
              as.character(adios.attrname),
              as.integer(length(data)),
              data)
    }
    
    invisible()
}

#' @title bp.flush
#' 
#' @description 
#' This function is used to close adios and clear custom environment
#'
#' @examples
#' \dontrun{
#' bp.flush()
#' }
#' 
#' @export
bp.flush <- function()
{
    adios.finalize()
    # Clear custom environment
    if (exists(".adiosenv", envir = .GlobalEnv)) {
        rm(list = ls(pos = .GlobalEnv$.adiosenv), envir = .GlobalEnv$.adiosenv)
    }
    
    invisible()
}
