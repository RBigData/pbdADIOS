
adios.read.init.method <- function(adios.read.method, 
                                   comm = pbdMPI:::.pbd_env$SPMD.CT$comm, 
                                   params)
{
    .Call("R_adios_read_init_method", 
        as.character(adios.read.method), 
        comm.c2f(comm), 
        as.character(params))
   invisible()
}

adios.read.open <- function(adios.filename, 
                            adios.read.method, 
                            comm = pbdMPI:::.pbd_env$SPMD.CT$comm, 
                            adios.lockmode, 
                            adios.timeout.sec)
{  
    .Call("R_adios_read_open", 
        as.character(adios.filename),
        as.character(adios.read.method), 
        comm.c2f(comm),
        as.character(adios.lockmode), 
        as.numeric(adios.timeout.sec))
}

adios.inq.var <- function(adios.file.ptr, 
                          adios.varname)
{
    .Call("R_adios_inq_var", 
          adios.file.ptr, 
          as.character(adios.varname))
}

adios.free.varinfo <- function(adios.varinfo)
{
    .Call("R_adios_free_varinfo", 
          adios.varinfo)
}

adios.inq.var.blockinfo <- function(adios.file.ptr, 
                                    adios.varinfo)
{
    .Call("R_adios_inq_var_blockinfo",
          adios.file.ptr, 
          adios.varinfo)
}

custom.inq.var.ndim <- function(adios.varinfo)
{
    .Call("R_custom_inq_var_ndim", 
          adios.varinfo)
}

custom.inq.var.dims <- function(adios.varinfo)
{
    .Call("R_custom_inq_var_dims", 
          adios.varinfo)
}

adios.selection.boundingbox <- function(adios.ndim, 
                                        adios.start, 
                                        adios.count){
    .Call("R_adios_selection_bounding_box", 
          as.integer(adios.ndim),
          as.numeric(adios.start), 
          as.numeric(adios.count))
}

adios.schedule.read <- function(adios.varinfo, 
                                adios.start, 
                                adios.count,
                                adios.file.ptr, 
                                adios.selection, 
                                adios.varname,
                                adios.from.steps, 
                                adios.nsteps)
{
    .Call("R_adios_schedule_read", 
          adios.varinfo, 
          as.numeric(adios.start),
          as.numeric(adios.count), 
          adios.file.ptr, 
          adios.selection,
          as.character(adios.varname), 
          as.integer(adios.from.steps),
          as.integer(adios.nsteps))
}

adios.perform.reads <- function(adios.file.ptr, 
                                adios.blocking){
    .Call("R_adios_perform_reads", 
          adios.file.ptr, 
          as.integer(adios.blocking))
}

custom.data.access <- function(adios.data, 
                               adios.selection, 
                               adios.varinfo){
    .Call("R_custom_data_access", 
          adios.data, 
          adios.selection,
          adios.varinfo)
}

adios.advance.step <- function(adios.file.ptr, 
                               adios.last, 
                               adios.timeout.sec){
    .Call("R_adios_advance_step", 
          adios.file.ptr,
          as.integer(adios.last), 
          as.numeric(adios.timeout.sec))
    invisible()
}

adios.read.close <- function(adios.file.ptr)
{
    .Call("R_adios_read_close", 
          adios.file.ptr)
    invisible()
}

adios.read.finalize.method<- function(adios.read.method)
{
    .Call("R_adios_read_finalize_method", 
          as.character(adios.read.method))
    invisible()
}

adios.errno<- function()
{
    .Call("R_adios_errno")
    invisible()
}
