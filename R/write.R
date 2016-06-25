#' @title ADIOS No-XML write init.
#' 
#' @param comm
#'
#' @export
adios.init.noxml <- function(comm = .pbd_env$SPMD.CT$comm)
{
    .Call("R_adios_init_noxml", 
          comm.c2f(comm))
    invisible()
}

#' @title Set maximum buffer size.
#' 
#' @param adios.max.buffersize
#'
#' @export
adios.set.max.buffersize <- function(adios.max.buffersize)
{
    .Call("R_adios_set_max_buffer_size", 
          as.numeric(adios.max.buffersize))
    invisible()
}

#' @title Declare a ADIOS group.
#' 
#' @param adios.groupname
#' @param adios.timeindex
#' @param adios.flag
#' 
#' @return adios group id pointer
#'
#' @export
adios.declare.group <- function(adios.groupname, 
                                adios.timeindex,
                                adios.flag)
{
    .Call("R_adios_declare_group", 
          as.character(adios.groupname), 
          as.character(adios.timeindex),
          as.character(adios.flag))
}

#' @title Select a I/O method for a ADIOS group.
#' 
#' @param adios.group_p
#' @param adios.method
#' @param adios.params
#' @param adios.basepath
#'
#' @export
adios.select.method <- function(adios.group_p, 
                                adios.method, 
                                adios.params, 
                                adios.basepath)
{
        
    .Call("R_adios_select_method", 
          adios.group_p, 
          as.character(adios.method), 
          as.character(adios.params), 
          as.character(adios.basepath))        
}

#' @title Define a ADIOS variable
#' 
#' @param adios.group_p
#' @param adios.varname
#' @param adios.path
#' @param adios.type
#' @param adios.localdim
#' @param adios.globaldim
#' @param adios.localoffset
#'
#' @return a variable ID, which can be used in adios_write_byid()
#'    0 return value indicates an error
#'
#' @export
adios.define.var <- function(adios.group_p, 
                             adios.varname, 
                             adios.path, 
                             adios.type,
                             adios.localdim, 
                             adios.globaldim, 
                             adios.localoffset)
{
  
    .Call("R_adios_define_var",
          adios.group_p, 
          as.character(adios.varname), 
          as.character(adios.path), 
          as.character(adios.type),
          as.character(adios.localdim), 
          as.character(adios.globaldim), 
          as.character(adios.localoffset))
}

#' @title Open or to append to an output file.
#' 
#' @description 
#' modes = "r" = "read", "w" = "write", "a" = "append", "u" = "update"
#' 
#' @param adios.groupname
#' @param adios.filename
#' @param adios.mode
#' @param comm
#'
#' @return adios file id pointer
#' 
#' @export
adios.open <- function(adios.groupname, 
                       adios.filename, 
                       adios.mode, 
                       comm = .pbd_env$SPMD.CT$comm)
{
  
    .Call("R_adios_open",
          as.character(adios.groupname), 
          as.character(adios.filename), 
          as.character(adios.mode), 
          comm.c2f(comm))
}

#' This function passes the size of the group to the internal ADIOS transport structure 
#' to facilitate the internal buffer management and to construct the group index table
#' 
#' @param adios.file_p
#' @param adios.groupsize
#'
#' @return R_adios_total_size
#' 
#' @export
adios.group.size <- function(adios.file_p, 
                             adios.groupsize)
{
    .Call("R_adios_group_size", 
          adios.file_p, 
          as.numeric(adios.groupsize))
}

#' @title Write the data either to internal buffer or disk.
#' 
#' @param adios.file_p
#' @param adios.varname
#' @param adios.var
#'
#' @export
adios.write <- function(adios.file_p, 
                        adios.varname, 
                        adios.var)
{
    .Call("R_adios_write",
          adios.file_p, 
          as.character(adios.varname), 
          adios.var)
    invisible()
}

#' @title Commit write/read operation and close the data.
#' 
#' @param adios.file_p
#'
#' @export
adios.close <- function(adios.file_p)
{
    .Call("R_adios_close", 
          adios.file_p)
}

#' @title Terminate ADIOS.
#' 
#' @param comm
#'
#' @export
adios.finalize <- function(comm = .pbd_env$SPMD.CT$comm)
{
    .Call("R_adios_finalize", 
          comm.rank(comm))
}
