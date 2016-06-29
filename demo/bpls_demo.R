### SHELL> mpiexec -np 4 Rscript bpls_demo.R


library(pbdMPI, quiet = TRUE)
library(pbdADIOS, quiet = TRUE)

init() # pbdMPI initilization

cat("rank ",comm.rank(.pbd_env$SPMD.CT$comm), "start\n")

bpls("heat.bp", .pbd_env$SPMD.CT$comm, comm.rank(.pbd_env$SPMD.CT$comm))

cat("rank ",comm.rank(.pbd_env$SPMD.CT$comm), "finish\n")

finalize() # pbdMPI final
