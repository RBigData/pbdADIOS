### SHELL> mpiexec -np 4 Rscript bpls_demo.R

library(pbdADIOS, quiet = TRUE)

init() # pbdMPI initilization

#cat("rank ",comm.rank(.pbd_env$SPMD.CT$comm), "start\n")

bpls("heat.bp")

#cat("rank ",comm.rank(.pbd_env$SPMD.CT$comm), "finish\n")

finalize() # pbdMPI final
