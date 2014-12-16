### ADIOS write function.

adios.write <- function(adios.handle, var.name, var){
  if(! is.null(adios.handle)){
    .Call("R_adios_write", adios.handle, as.character(var.name), var)
  }
  
  if(ret == -1 )
  {  
    pbdMPI::comm.print("Error: unknown datatypes(Only supported int and real)")
  }  
} # End of adios.write()
