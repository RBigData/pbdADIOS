### SHELL> mpiexec -np 4 Rscript Write_demo.R
library(pbdDMAT, quiet = TRUE)
library(pbdADIOS, quiet = TRUE)


NX <- 20
t<- c()

O <- 19 ## ??
groupname <- "restart"

#type <- c(2,2,2,6)

#init() ##pbdMPI init

filename <- "test_write.bp"

G <- NX * pbdMPI::comm.size()
O <- pbdMPI::comm.rank() * NX + NX

for(i in 1:NX){
  t[i] <- pbdMPI::comm.rank() + (i * 0.1) + 0.01
}

pbdADIOS:::adios.init.noxml() ## implicitly using comm
pbdADIOS:::adios.set.max.buffersize(200) ## Buffer size is in MB
adios_group_ptr <-  pbdADIOS:::adios.declare.group(groupname,"", "adios_flag_yes") 

pbdADIOS:::adios.select.method(adios_group_ptr, "MPI", "", "")

#adios.define.var(adios_group_ptr,"temperature","", toString(NX), toString(G), toString(O)) 
pbdADIOS:::adios.define.var(adios_group_ptr,"temperature","", "adios_double", 20, 40, 40)


print("done with define var")

num_timestamp <- 5
for(j in 1:num_timestamp) {

    adios_file_ptr <- pbdADIOS:::adios.open(groupname, filename, "a") ## Comm is implicit
    groupsize <- NX * 8 + NX * 8 ## Can be done in backend

    pbdADIOS:::adios.group.size(adios_file_ptr, groupsize) ## Has return value but don't need to use

    pbdADIOS:::adios.write(adios_file_ptr, "temperature", t)

    pbdADIOS:::adios.close(adios_file_ptr)
    barrier()
}

pbdADIOS:::adios.finalize() # ADIOS finalize
finalize() # pbdMPI final
