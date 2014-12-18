
# Pragnesh Patel
# ADIOS streaming write function.

## GO: this file needs parameters to match R_adios_write.c

adios.write.open <-function(comm = pbdMPI::.SPMD.CT$comm, group_name,
                            transport_method, filename, mode){
    adios.info <- .Call("R_adios_write_open", comm.c2f(comm),
                        as.character(group_name), as.character(transport_method),
                        as.character(filename), as,character(mode))
   invisible(adios.info)
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





