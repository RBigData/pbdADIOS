# mpirun -np 1 Rscript bp.stage.read.demo.R

library(pbdADIOS, quiet = TRUE)

# user defined function for bp.stage.read
test.fun <- function(x) {
    # calculate the number of variables
    nvars = length(x)
    for(i in 1:nvars) {
        comm.print(names(x)[i], all.rank=TRUE)
        comm.print(x[[i]], all.rank=TRUE)
    }
}

bp.stage.read("arrays", list("var_2d_array", "test_scalar"), test.fun)

finalize() # pbdMPI final