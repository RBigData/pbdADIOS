library(pbdMPI, quiet = TRUE)
library(pbdADIOS, quiet = TRUE)
library(raster, quiet=TRUE, warn.conflicts=FALSE)
library(ggplot2, quiet=TRUE)
library(grid, quiet=TRUE, warn.conflicts=FALSE)

## begin function definitions
raster_plot <- function(x, nrow, ncol, bname="raster", n=1, sw=3)
{
    x <- data.frame(rasterToPoints(raster(matrix(x, nrow, ncol),
                                          xmn=0, xmx=ncol, ymn=0, ymx=nrow)))
    names(x) <- c("x", "y", bname)
    png(paste(bname, "_", formatC(n, width=sw, flag=0), "_", comm.rank(),
              ".png", sep=""))
    print(
      ggplot(x, aes_string("x", "y", fill=bname)) + geom_raster() +
          scale_fill_gradient2(limits=range(x))
         )
    dev.off()
}
## end function definitions

## Initialize MPI and ADIOS
pbdMPI::init() # MPI intializer
adios.init.noxml() # Write without using XML # Wr
adios.read.init.method("ADIOS_READ_METHOD_BP", params="verbose=3") # Init read

## specify and open file for reading
file_r <- "heat.bp"
timeout.read <- 1 ## in seconds
read.file.ptr <- adios.read.open(file_r, adios.timeout=timeout.read, 
                                 "ADIOS_READ_METHOD_BP",
                                 adios.lockmode="ADIOS_LOCKMODE_NONE")
## select variable to read and write
variable_r <- "T"
variable_w <- "quad"

## get variable dimensions
varinfo = adios.inq.var(read.file.ptr, variable_r)
block <- adios.inq.var.blockinfo(read.file.ptr, varinfo)
ndim <- custom.inq.var.ndim(varinfo)
dims <- custom.inq.var.dims(varinfo)

## split over the first dimension
g.dim <- dims # global.dim on write
my.i <- get.jid(g.dim[1])
my.start <- c(my.i[1] - 1, 0) # adjust R to C start
my.dim <- my.count <- c(length(my.i), g.dim[2])

adios.allocate.buffer(300) # allocating size for ADIOS application in MB # Wr

groupname <- "restart" ## Wr
adios_group_ptr <- adios.declare.group(groupname, "") ## Wr

adios.select.method(adios_group_ptr, "MPI", "", "") ## Wr
file_w <- "heat_R.bp" ## Wr

adios.define.var(adios_group_ptr, variable_w, "", toString(my.dim), 
                 toString(g.dim), toString(my.start))  ## Wr

## Initialize and allocate
errno <- 0
steps <- 0
bufsize <- 10
buffer <- matrix(NA, ncol=prod(my.count), nrow=bufsize)

## create orhogonal deg-2 regression matrix
rhs <- cbind(rep(1, bufsize), poly(1:bufsize, degree=2))

while(errno != -21) { ##  -21=err_end_of_stream
    steps <- steps + 1

    ## set reading bounding box
    adios.selection  <- adios.selection.boundingbox(ndim, my.start, my.count)
    
    ## schedule the read
    adios.data <- adios.schedule.read(varinfo, my.start, my.count, 
                                      read.file.ptr, adios.selection,
                                      variable_r, 0, 1)
    ## perform the read
    adios.perform.reads(read.file.ptr, 1)
    data_chunk <- custom.data.access(adios.data, adios.selection, varinfo)

    ## maintain a moving buffer of time steps
    buffer <- rbind(buffer[-1, ], data_chunk)

    ## fit data and plot coefficient raster plots
    if(steps >= bufsize)
        {
            ## Fit a quadratic to a moving window of 10 steps
            fit <- lm.fit(rhs, buffer)
            coef <- fit$coefficients
            errs <- apply(fit$residuals, 2, function(x) max(abs(x)))

            ## Plot coef and errs
            raster_plot(coef[2, ], my.dim[2], my.dim[1], "lin", steps)
            raster_plot(coef[3, ], my.dim[2], my.dim[1], "quad", steps)
            raster_plot(errs, my.dim[2], my.dim[1], "maxerr", steps)

            ## Write one quadratic coefficient time step
            adios_file_ptr <- adios.open(groupname, file_w, "a") ## Wr
            groupsize <- object.size(data_chunk) ## Wr
            adios.group.size(adios_file_ptr, groupsize) ## Wr
            adios.write(adios_file_ptr, variable_w, coef[3, ]) ## Wr
            adios.close(adios_file_ptr) ## Wr
            barrier() ## Wr
        }
    
    ## try to get more data
    adios.advance.step(read.file.ptr, 0, adios.timeout.sec=1)
    comm.print(paste("Done advance.step", steps, "..."))
     
    ## collect error for possible timeout end of file
    errno <- adios.errno()
    if(steps > 18) break
} 

adios.read.close(read.file.ptr)
adios.read.finalize.method("ADIOS_READ_METHOD_BP")
adios.finalize(comm.rank()) # ADIOS finalize ## Wr
finalize() # pbdMPI finalize

