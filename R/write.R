adios.init.noxml <- function(comm = pbdMPI:::.pbd_env$SPMD.CT$comm)
{
    .Call("R_adios_init_noxml", 
          comm.c2f(comm))
    invisible()
}

adios.set.max.buffersize <- function(adios.max.buffersize)
{
    .Call("R_adios_set_max_buffer_size", 
          as.numeric(adios.max.buffersize))
    invisible()
}

adios.declare.group <- function(adios.groupname, 
                                adios.timeindex,
                                adios.flag)
{
    .Call("R_adios_declare_group", 
          as.character(adios.groupname), 
          as.character(adios.timeindex),
          as.character(adios.flag))
}

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

adios.define.var <- function(adios.group_p, 
                             adios.varname, 
                             adios.path, 
                             adios.type,
                             adios.localdim, 
                             adios.globaldim, 
                             adios.localoffset )
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

adios.open <- function(adios_fd,
                       adios.groupname, 
                       adios.filename, 
                       adios.mode, 
                       comm = pbdMPI:::.pbd_env$SPMD.CT$comm)
{
  
    .Call("R_adios_open",
          adios_fd,
          as.character(adios.groupname), 
          as.character(adios.filename), 
          as.character(adios.mode), 
          comm.c2f(comm))
}

adios.group.size <- function(adios.file_p, 
                             adios.groupsize)
{
    .Call("R_adios_group_size", 
          adios.file_p, 
          as.numeric(adios.groupsize))
}

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

adios.close <- function(adios.file_p)
{
    .Call("R_adios_close", 
          adios.file_p)
}

adios.finalize <- function(comm = pbdMPI:::.pbd_env$SPMD.CT$comm)
{
    .Call("R_adios_finalize", 
          comm.rank(comm))
}
