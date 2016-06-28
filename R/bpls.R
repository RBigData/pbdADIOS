#' @title print variables and attributes
#' 
#' @param adios.filename
#'
#' @export
bpls <- function(adios.filename)
{
    .Call("R_bpls", 
          as.character(adios.filename))
    invisible()
}