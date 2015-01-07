### Lastest load into a package.

.First.lib <- function(lib, pkg){
  if(! is.loaded("spmd_initialize", PACKAGE = "pbdMPI")){
    library.dynam("pbdMPI", "pbdMPI", lib)
    if(pbdMPI::comm.is.null(0L) == -1){
      pbdMPI::init()
    }
  }

  library.dynam("pbdADIOS", pkg, lib)
} # End of .First.lib().

.Last.lib <- function(libpath){
  pbdADIOS::adios.finalize(pbdMPI::comm.rank())
  library.dynam.unload("pbdADIOS", libpath)
} # End of .Last.lib().
