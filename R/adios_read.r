#' @title Read Initialization
#' 
#' @description 
#' TODO a quick description
#' 
#' @details
#' TODO more details
#' 
#' @param adios.read.method 
#' TODO
#' @param comm
#' TODO
#' @param params
#' TODO
#' @param adios.filename
#' TODO
#' @param adios.lockmode
#' TODO
#' @param adios.timeout.sec
#' TODO
#' @param adios.file.ptr
#' TODO
#' 
#' @examples \dontrun{
#' TODO
#' }
#' 
#' @name adios.read
#' @rdname adios.read
NULL


#' @rdname adios.read
#' @export
adios.read.init.method <- function(adios.read.method, comm = pbdMPI::.SPMD.CT$comm, params)
{
  # TODO comm.match.arg for adios.read.method
  .Call(R_adios_read_init_method, as.character(adios.read.method), comm.c2f(comm), as.character(params))
   invisible()
}

#' @rdname adios.read
#' @export
adios.read.open <- function(adios.filename, adios.read.method, comm = pbdMPI::.SPMD.CT$comm, adios.lockmode, adios.timeout.sec)
{  
  .Call(R_adios_read_open, as.character(adios.filename),
      as.character(adios.read.method), comm.c2f(comm),
      as.character(adios.lockmode), as.numeric(adios.timeout.sec))
}

#' @rdname adios.read
#' @export
adios.read.close <- function(adios.file.ptr)
{
  .Call(R_adios_read_close, adios.file.ptr)
  invisible()
}

#' @rdname adios.read
#' @export
adios.read.finalize.method<- function(adios.read.method)
{
  .Call(R_adios_read_finalize_method, as.character(adios.read.method))
   invisible()
}
