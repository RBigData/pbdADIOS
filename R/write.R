#' @title R wrapper of ADIOS write API
#' 
#' @description 
#' For more details of these APIs, please refer to https://github.com/ornladios/ADIOS/blob/master/src/public/adios.h

#' @rdname adios.write.api
adios.init.noxml <- function(comm = .pbd_env$SPMD.CT$comm)
{
    .Call("R_adios_init_noxml", 
          comm.c2f(comm))
    invisible()
}

#' @rdname adios.write.api
adios.set.max.buffersize <- function(adios.max.buffersize)
{
    .Call("R_adios_set_max_buffer_size", 
          as.numeric(adios.max.buffersize))
    invisible()
}

#' @rdname adios.write.api
adios.declare.group <- function(adios.groupname, 
                                adios.timeindex,
                                adios.flag)
{
    .Call("R_adios_declare_group", 
          as.character(adios.groupname), 
          as.character(adios.timeindex),
          as.character(adios.flag))
}

#' @rdname adios.write.api
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

#' @rdname adios.write.api
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

#' @rdname adios.write.api
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

#' @rdname adios.write.api
adios.group.size <- function(adios.file_p, 
                             adios.groupsize)
{
    .Call("R_adios_group_size", 
          adios.file_p, 
          as.numeric(adios.groupsize))
}

#' @rdname adios.write.api
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

#' @rdname adios.write.api
adios.close <- function(adios.file_p)
{
    .Call("R_adios_close", 
          adios.file_p)
}

#' @rdname adios.write.api
adios.finalize <- function(comm = .pbd_env$SPMD.CT$comm)
{
    .Call("R_adios_finalize", 
          comm.rank(comm))
}
