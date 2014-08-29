## Pragnesh Patel ##

### SHELL> mpiexec -np 4 Rscript --vanilla test_read.r
#library(pbdADIOS, quiet = TRUE)

library(pbdMPI, quiet = TRUE)

dir <- ("/ccs/home/ost/adios/test")
dyn.load(paste(dir, "src/pbdADIOS_stream.so", sep="/"))

## Include R interface file for ADIOS
source(paste(dir, "R1/adios_read_str.r", sep="/"))

init()

adios.read.init.method("ADIOS_READ_METHOD_BP", params="verbose=3")
timeout.sec <- 1.0

dir.data <- "/lustre/atlas/scratch/ost/mat044/adios_test/data"
file <- paste(dir.data, "bench.s000.b0.bp", sep="/")
file.ptr <- adios.read.open(file, "ADIOS_READ_METHOD_BP",
                                  adios.lockmode="ADIOS_LOCKMODE_NONE",
                                  adios.timeout.sec=timeout.sec)  

varinfo = adios.inq.var(file.ptr, "/ion0/LocalECP")
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

while(errno != -21) { ## This is hard-coded for now. -21=err_end_of_stream
    steps = steps + 1 ## Double check with Norbert. Should it start with 1 or 2

    adios.selection  <- adios.selection.boundingbox(ndim, start, count)
    comm.print("Done adios.selection.boundingbox method")
    ## cat("Steps", steps)
    
    ## adios.datatype <- "double" 
    ## adios.data would not have any data until you do adios.perform.reads
    adios.data <- adios.schedule.read(varinfo, start, count,
                                      file.ptr, adios.selection,
                                      "/ion0/LocalECP", 0, 1)

    adios.perform.reads(file.ptr,1)
    comm.print("Done perform_reads and printing results")

    cat("\nSlice size is", slice_size,"\n")
    data_chunk <- custom.data.access(adios.data, adios.selection, varinfo)

    ## print a few to verify
    n_data <- length(data_chunk)
    comm.cat("first 10:", head(data_chunk, 10),"\n", all.rank=TRUE)
    comm.cat("last 10:", tail(data_chunk, 10),"\n", all.rank=TRUE)

    ## check if more data came in
    adios.advance.step(file.ptr, 0, adios.timeout.sec=timeout.sec)
    comm.print("Done adios.advance.step method")
    
    errno <- adios.errno()
    cat("Error Num",errno)

    if(errno == -22){ #-22 = err_step_notready
        cat("\n No new step arrived within the timeout. Quit.\n")
        break;    
    }

} # While end 
  
adios.read.close(file.ptr)
comm.print("We have processed steps")
adios.read.finalize.method("ADIOS_READ_METHOD_BP")

finalize() # pbdMPI final
