#' @title read init
#' 
#' @description 
#' Initialize a reading method before opening a file/stream with using the method.
#' 
#' @param adios.read.method
#' @param comm
#' @param params
#'
#' @export
adios.read.init.method <- function(adios.read.method, 
                                   comm = .pbd_env$SPMD.CT$comm, 
                                   params)
{
    .Call("R_adios_read_init_method", 
        as.character(adios.read.method), 
        comm.c2f(comm), 
        as.character(params))
    invisible()
}

#' @title Open an adios file/stream as a stream.
#' 
#' @description 
#' Only one step at a time can be read. The list of variables will change when
#' advancing the step if the writing application writes different variables at
#' different times.
#' 
#' @param adios.filename
#' @param adios.read.method
#' @param comm
#' @param adios.lockmode
#' @param adios.timeout.sec
#'
#' @return adios file pointer
#' 
#' @export
adios.read.open <- function(adios.filename, 
                            adios.read.method, 
                            comm = .pbd_env$SPMD.CT$comm, 
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

#' @title Inquiry about a variable.
#' 
#' @description 
#' This function does not read anything from the file but processes info
#' already in memory after fopen.
#' 
#' @param adios.file.ptr
#' @param adios.varname
#' 
#' @return adios_var_info pointer
#' 
#' @export
adios.inq.var <- function(adios.file.ptr, 
                          adios.varname)
{
    .Call("R_adios_inq_var", 
          adios.file.ptr, 
          as.character(adios.varname))
}

#' @title Inquiry a variable by index.
#' 
#' @param adios.file.ptr
#' @param adios.varid
#' 
#' @return adios_var_info pointer
#' 
#' @export
adios.inq.var.byid <- function(adios.file.ptr, 
                               adios.varid)
{
    .Call("R_adios_inq_var_byid", 
          adios.file.ptr, 
          as.integer(adios.varid))
}

#' @title Free memory used by an ADIOS_VARINFO struct.
#' 
#' @param adios.varinfo
#'
#' @export
adios.free.varinfo <- function(adios.varinfo)
{
    .Call("R_adios_free_varinfo", 
          adios.varinfo)
    invisible()
}

#' @title Get the block-decomposition of the variable about how it is stored in 
#' the file or stream. 
#' 
#' @param adios.file.ptr
#' @param adios.varinfo
#'
#' @export
adios.inq.var.blockinfo <- function(adios.file.ptr, 
                                    adios.varinfo)
{
    .Call("R_adios_inq_var_blockinfo",
          adios.file.ptr, 
          adios.varinfo)
}

#' @title Get number of dimensions.
#' 
#' @param adios.varinfo
#'
#' @return number of dimensions
#' 
#' @export
custom.inq.var.ndim <- function(adios.varinfo)
{
    .Call("R_custom_inq_var_ndim", 
          adios.varinfo)
}

#' @title Get size of each dimension.
#' 
#' @param adios.varinfo
#'
#' @return size of each dimensions
#' 
#' @export
custom.inq.var.dims <- function(adios.varinfo)
{
    .Call("R_custom_inq_var_dims", 
          adios.varinfo)
}

#' @title adios_selection_bounding_box API.
#' 
#' @param adios.ndim
#' @param adios.start
#' @param adios.count
#'
#' @return adios_selection pionter
#' 
#' @export
adios.selection.boundingbox <- function(adios.ndim, 
                                        adios.start, 
                                        adios.count){
    .Call("R_adios_selection_boundingbox", 
          as.integer(adios.ndim),
          as.numeric(adios.start), 
          as.numeric(adios.count))
}

#' @title Schedule reading a variable (slice) from the file.
#' 
#' @description 
#' You need to call adios.perform.reads to do the reading.
#' 
#' @param adios.varinfo
#' @param adios.start
#' @param adios.count
#' @param adios.file.ptr
#' @param adios.selection
#' @param adios.varname
#' @param adios.from.steps
#' @param adios.nsteps
#'
#' @return pointer to the memory to hold dsata of the variable.
#' 
#' @export
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

#' @title Let ADIOS perform the scheduled reads.
#' 
#' @param adios.file.ptr
#' @param adios.blocking
#'
#' @export
adios.perform.reads <- function(adios.file.ptr, 
                                adios.blocking){
    .Call("R_adios_perform_reads", 
          adios.file.ptr, 
          as.integer(adios.blocking))
}

#' @title Copy the scheduled reads from C object to R object.
#' 
#' @param adios.data
#' @param adios.selection
#' @param adios.varinfo
#'
#' @return the data read by adios
#' 
#' @export
custom.data.access <- function(adios.data, 
                               adios.selection, 
                               adios.varinfo){
    .Call("R_custom_data_access", 
          adios.data, 
          adios.selection,
          adios.varinfo)
}

#' @title Advance the current step of a stream.
#' 
#' @description 
#' For files opened as file, stepping has no effect.
#' 
#' @param adios.file.ptr
#' @param adios.last
#' @param adios.timeout.sec
#'
#' @export
adios.advance.step <- function(adios.file.ptr, 
                               adios.last, 
                               adios.timeout.sec){
    .Call("R_adios_advance_step", 
          adios.file.ptr,
          as.integer(adios.last), 
          as.numeric(adios.timeout.sec))
}

#' @title Close an adios file.
#' 
#' @description 
#' It will free the content of the underlying data structures and the fp pointer itself.
#' 
#' @param adios.file.ptr
#'
#' @export
adios.read.close <- function(adios.file.ptr)
{
    .Call("R_adios_read_close", 
          adios.file.ptr)
    invisible()
}

#' @title Finalize the selected method.
#' 
#' @description 
#' Required for all methods that are initialized. 
#' 
#' @param adios.read.method
#'
#' @export
adios.read.finalize.method<- function(adios.read.method)
{
    .Call("R_adios_read_finalize_method", 
          as.character(adios.read.method))
    invisible()
}

#' @title Get error number
#'
#' @return error number
#' 
#' @export
adios.errno<- function()
{
    .Call("R_adios_errno")
}
