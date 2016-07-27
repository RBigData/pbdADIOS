### SHELL> mpiexec -np 4 Rscript Read_demo.R

library(pbdADIOS, quiet = TRUE)

#init()

adios.read.init.method("ADIOS_READ_METHOD_BP", params="verbose=3")
timeout.sec <- 1.0

file <- "heat.bp"
file.ptr <- adios.read.open(file, "ADIOS_READ_METHOD_BP",
                                  adios.lockmode="ADIOS_LOCKMODE_NONE",
                                  adios.timeout.sec=timeout.sec)  

varinfo = adios.inq.var(file.ptr, "T")
adios.inq.var.blockinfo(file.ptr, varinfo)
ndim <- custom.inq.var.ndim(varinfo)
dims <- custom.inq.var.dims(varinfo)
comm.cat("ndim", ndim, "dims", dims, "\n")

## R index starts with 1 not 0
## slice_size = num rows per rank
slice_size <- as.integer(dims[1] %/% comm.size())
if(comm.rank() == (comm.size() - 1)) {
    slice_size <- as.integer(slice_size + (dims[1] %% comm.size()))
}

start <- c(as.integer(comm.rank() * slice_size), 0)
count <- c(slice_size, as.integer(dims[2]))
cat(comm.rank(), "slice_size", slice_size, "start", start, "count", count, "\n")

errno <- 0 # Default value 0
steps <- 0
retval <- 0

if(errno != -21) { ## This is hard-coded for now. -21=err_end_of_stream
    steps = steps + 1 ## Double check with Norbert. Should it start with 1 or 2

    adios.selection  <- pbdADIOS:::adios.selection.boundingbox(ndim, start, count)
    comm.print("Done adios.selection.boundingbox method")
    ## cat("Steps", steps)
    
    ## adios.datatype <- "double" 
    ## adios.data would not have any data until you do adios.perform.reads
    adios.data <- pbdADIOS:::adios.schedule.read(varinfo, start, count,
                                      file.ptr, adios.selection,
                                      "T", 0, 1)

    pbdADIOS:::adios.perform.reads(file.ptr,1)
    comm.print("Done perform_reads and printing results")

    cat("\nSlice size is", slice_size,"\n")
    data_chunk <- pbdADIOS:::custom.data.access(adios.data, adios.selection, varinfo)

    ## print a few to verify
    n_data <- length(data_chunk)
    comm.cat("first 10:", head(data_chunk, 10),"\n", all.rank=TRUE)
    comm.cat("last 10:", tail(data_chunk, 10),"\n", all.rank=TRUE)

    ## check if more data came in
    pbdADIOS:::adios.advance.step(file.ptr, 0, adios.timeout.sec=timeout.sec)
    comm.print("Done adios.advance.step method")
    
    errno <- pbdADIOS:::adios.errno()
    cat("Error Num",errno)

    if(errno == -22){ #-22 = err_step_notready
        cat("\n No new step arrived within the timeout. Quit.\n")
        break;    
    }

} # While end 

pbdADIOS:::adios.free.varinfo(varinfo)

pbdADIOS:::adios.read.close(file.ptr)
comm.print("We have processed steps")
pbdADIOS:::adios.read.finalize.method("ADIOS_READ_METHOD_BP")

finalize() # pbdMPI final