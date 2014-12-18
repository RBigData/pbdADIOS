
# Pragnesh Patel
# ADIOS streaming write function.

## GO: this file needs parameters to match SGN_R_adios_write_wrapper.c

adios.write.open <-function(){
  .Call("R_adios_write_open", )
   invisible()
}

adios.write.close <- function(..., comm = pbdMPI::.SPMD.CT$comm, ){
    dots <- match.call(expand.dots = FALSE)$...
    if (length(dots) && !all(sapply(dots, is.symbol))) 
        stop("adios.write.close: ... must contain names")
    for(i in seq_along(dots))
        {
            ## loop over each dots parameter
            ## get(as.character(dots[[i]]))
        }
    
  .Call("R_adios_write_close", comm.c2f(comm), )
   #invisible()                                                               
}

adios.finalize <- function(comm = pbdMPI::.SPMD.CT$comm){
  .Call("R_adios_finalize", comm.rank(comm))
  invisible()
} # End of adios.finalize() 





