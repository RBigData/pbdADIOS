#' @export
.Last.lib <- function(libpath){
  pbdADIOS::adios.finalize(pbdMPI::comm.rank())
} 
