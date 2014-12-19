
# Pragnesh Patel
# ADIOS streaming write function.

## GO: this file needs parameters to match R_adios_write.c

adios.write.open <-function(filename, group_name, transport_method, mode,
                            comm = pbdMPI::.SPMD.CT$comm){
    adios.info <- .Call("R_adios_write_open", comm.c2f(comm),
                        as.character(group_name), as.character(transport_method),
                        as.character(filename), as,character(mode))
   invisible(adios.info)
}

adios.write.close <- function(..., comm = pbdMPI::.SPMD.CT$comm, ){
    dots <- match.call(expand.dots = FALSE)$...
    if (length(dots) && !all(sapply(dots, is.symbol))) 
        stop("adios.write.close: ... must contain names")
    varnames <- sapply(dots, as.character)
    numvars <- length(varnames)
    type <- sapply(varnames, function(x) typeof(get(x)))
    local_dim <- sapply(varnames, function(x) dim(get(x)))
    
    for(i in seq_along(dots))
        {
            ## loop over each dots parameter
            ## get(as.character(dots[[i]]))
            typeof(get(as.character(dots[[1]])))
        }
    
    .Call("R_adios_write_close", comm.c2f(comm), as.int(numvars),
          as.character(varnames), as.integer(type), as.character(local_dim),
          as.character(global_dim), as.character(local_offset),	R_adios_file_group *R_adios_info, void **data)
   #invisible()                                                               
}

adios.finalize <- function(comm = pbdMPI::.SPMD.CT$comm){
  .Call("R_adios_finalize", comm.rank(comm))
  invisible()
} # End of adios.finalize() 





