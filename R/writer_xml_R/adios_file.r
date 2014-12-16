### ADIOS file functions.

adios.open <- function(bp.file, comm = .SPMD.CT$comm){
  .Call("R_adios_open", as.character(bp.file), comm.c2f(comm.rank(comm)))
} # End of adios.init()

adios.close <- function(adios.handle){
  if(! is.null(adios.handle)){
    .Call("R_adios_close", adios.handle)
  }
  invisible()
} # End of adios.close()
