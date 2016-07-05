### SHELL> mpiexec -np 4 Rscript dump_demo.R

library(pbdADIOS, quiet = TRUE)

bp.dump("attributes.bp")

finalize() # pbdMPI final
