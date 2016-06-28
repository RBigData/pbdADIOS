### SHELL> mpiexec -np 4 Rscript bpls_demo.R


library(pbdMPI, quiet = TRUE)
library(pbdADIOS, quiet = TRUE)

init() # pbdMPI initilization

bpls("heat.bp")

finalize() # pbdMPI final
