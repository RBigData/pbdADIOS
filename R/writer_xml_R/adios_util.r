### ADIOS utility functions.

adios.init <- function(xml.file){
  .Call("R_adios_init", as.character(xml.file))
  invisible()
} # End of adios.init()

adios.finalize <- function(comm = .SPMD.CT$comm){
  .Call("R_adios_finalize", comm.rank(comm))
  invisible()
} # End of adios.finalize()
