### SHELL> mpiexec -np 4 Rscript --vanilla test_write.r
library(pbdMPI, quiet = TRUE)
library(pbdDMAT, quiet = TRUE)
library(pbdADIOS, quiet = TRUE)
#library(raster, quiet=TRUE)
#library(ggplot2, quiet=TRUE)
#library(grid, quiet=TRUE)


## Remember this demo is C style in R. You may find weird data structures.

NX <- 20
t<- c()

O <- 19 ## ??
groupname <- "restart"

#type <- c(2,2,2,6)

init() ##pbdMPI init

filename <- "/Users/pragnesh/5.1.1/SGN_pbdADIOS/pbdADIOS_22_dec/pbdADIOS/tests/test_write.bp"

G <- NX * pbdMPI:::comm.size()
O <- pbdMPI:::comm.rank() * NX + NX

for(i in 1:NX){
  t[i] <- pbdMPI:::comm.rank() + (i * 0.1) + 0.01
}

adios.init.noxml() ## implicitly using comm
adios.allocate.buffer(200) ## Buffer size is in MB
adios_group_ptr <-  adios.declare.group(groupname,"") 

adios.select.method(adios_group_ptr, "MPI", "", "")

#adios.define.var(adios_group_ptr,"temperature","", toString(NX), toString(G), toString(O)) 
adios.define.var(adios_group_ptr,"temperature","", 20, 40, 40)


print("done with define var")

num_timestamp <- 5
for(j in 1:num_timestamp) {

      adios_file_ptr <- adios.open(groupname, filename, "a") ## Comm is implicit
      groupsize <- NX * 8 + NX * 8 ## Can be done in backend

      adios.group.size(adios_file_ptr, groupsize) ## Has return value but don't need to use

      adios.write(adios_file_ptr, "temperature", t)

      adios.close(adios_file_ptr)
      barrier()

}

adios.finalize(pbdMPI:::comm.rank()) # ADIOS finalize
finalize() # pbdMPI final
