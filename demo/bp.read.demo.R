# parallel read example
# mpirun -np 2 Rscript bp.write.demo.R

# load pbdADIOS lib
library(pbdADIOS, quiet = TRUE)

A = bp.read("heat.bp", "T", c(0, 0, 0), c(1, 1, 20));
comm.print(A, all.rank=TRUE)

finalize() # pbdMPI final
