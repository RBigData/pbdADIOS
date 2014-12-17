
# Pragnesh Patel
# ADIOS streaming write function.


adios.read.init.method <-function(adios.read.method, comm = .SPMD.CT$comm, params){
  .Call("R_adios_read_init_method",as.character(adios.read.method), comm.c2f(comm), as.character(params))
   invisible()
}

adios.read.open <- function(adios.filename, adios.read.method, comm= .SPMD.CT$comm, adios.lockmode, adios.timeout.sec){
  .Call("R_adios_read_open",as.character(adios.filename), as.character(adios.read.method),comm.c2f(comm), as.character(adios.lockmode),as.numeric(adios.timeout.sec))
   #invisible()                                                               
}





adios.finalize <- function(comm = .SPMD.CT$comm){
  .Call("R_adios_finalize", comm.rank(comm))
  invisible()
} # End of adios.finalize() 





