#' @title ???
#' 
#' @description 
#' TODO a quick description
#' 
#' @details
#' TODO more details
#' 
#' @param adios.file.ptr
#' TODO
#' @param adios.varname
#' TODO
#' @param adios.varinfo
#' TODO
#' 
#' @examples \dontrun{
#' TODO
#' }
#' 
#' @name adios.inq.var
#' @rdname adios.inq.var
NULL

#' @rdname adios.inq.var
#' @export
adios.inq.var <- function(adios.file.ptr, adios.varname)
{
  .Call(R_adios_inq_var, adios.file.ptr, as.character(adios.varname))
}

#' @rdname adios.inq.var
#' @export
custom.inq.var.ndim <- function(adios.varinfo)
{
  .Call(R_custom_inq_var_ndim, adios.varinfo)
}

#' @rdname adios.inq.var
#' @export
custom.inq.var.dims <- function(adios.varinfo)
{
  .Call(R_custom_inq_var_dims, adios.varinfo)
}

#' @rdname adios.inq.var
#' @export
adios.inq.var.blockinfo <- function(adios.file.ptr, adios.varinfo)
{
  .Call(R_adios_inq_var_blockinfo,adios.file.ptr, adios.varinfo)
}
