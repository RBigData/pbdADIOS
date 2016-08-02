# mpirun -np 2 Rscript bp.write.demo.R

# load pbdADIOS lib
library(pbdADIOS, quiet = TRUE)

# create bp file
bp.create("bptest.bp")

# add attributes to the bp file
today <- Sys.Date()
bp.attr("date", format(today, format="%B %d %Y"))
bp.attr("README", "This is just a test.")

# define variables and write data

rank = comm.rank()
bp.var("T", (rank+1)*c(1:6))

a <- matrix((rank+1)*c(1:6), ncol = 3, nrow = 2)
bp.var("a", a)

b <- array((rank+1)*c(1:6), c(2, 1, 3))
bp.var("b", b)

bp.write()

for(i in 2:3) {
    # append data
    bp.var("T", i*(rank+1)*c(1:6))

    a <- matrix(i*(rank+1)*c(1:6), ncol = 3, nrow = 2)
    bp.var("a", a)

    b <- array(i*(rank+1)*c(1:6), c(2, 1, 3))
    bp.var("b", b)

    bp.write()
}

# close adios
bp.flush()

finalize() 