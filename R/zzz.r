.onUnload <- function(libpath)
{
  pbdADIOS::adios.finalize(pbdMPI::comm.rank())
} 
