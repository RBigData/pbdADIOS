### ADIOS write function.

adios.group.size <- function(adios.handle, adios.groupsize, adios.totalsize){
  if(! is.null(adios.handle)){
    .Call("R_adios_group_size", adios.handle, as.integer(adios.groupsize), as.integer(adios.totalsize))
  }

  pbdMPI::comm.print("DEBUG: returning from R_adios_group_size in R")
  #if(ret == -1 )
  #{  
  #  pbdMPI::comm.print("Error: unknown datatypes(Only supported int and real)")
  #}  
} # End of adios.group.size()
