## Pragnesh Patel ##
### SHELL> mpiexec -np 4 Rscript --vanilla test_read.r


## Remember this demo is in C style in R. You may find weird data structures.

##
## GO: extract writing parts and add to reading test script
##

library(pbdMPI, quiet = TRUE)
## Change location, if needed
dyn.load("/Users/pragnesh/5.1.1/SGN_pbdADIOS/pbdADIOS/src/sgn_writer/pbdADIOS_write.so")

## Source R interface file for ADIOS
source("adios_write_str.r")

## Define vars
varnames <- c("NX","G","O","temperature")
numvars <- length(varnames)

type <- c(2,2,2,6)

local_dim <- c(0,0,0,"NX")
global_dim <- c(0,0,0,"G")
local_offset <- c(0,0,0,"O")

NX <- 20
t<- c()

## pbdMPI init
init()

for(i in 1:NX){
  t[i] <- pbdMPI:::comm.rank() + (i * 0.1) + 0.01
}

#print(t)
G <- NX * pbdMPI:::comm.size()

###########
num_timestamp <- 5
for(j in 1:num_timestamp) {
      
#    Pass comm     
     R_adios_info <- adios.write.open(comm,"restart","MPI","/Users/pragnesh/5.1.1/SGN_pbdADIOS/dataset/adios_write_test.bp","a");

    O <- pbdMPI:::comm.rank() * 2 * NX + NX;

    ## Combine all data into one

#    adios.write.close()
 
}
###########

print(O)


adios.finalize(pbdMPI:::comm.rank()) # ADIOS finalize
finalize() # pbdMPI final
