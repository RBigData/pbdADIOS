# mpirun -np 2 Rscript bp.write_demo.R

# load pbdADIOS lib
library(pbdADIOS, quiet = TRUE)

# create bp file
bp.create("bptest.bp")

# add attributes to the bp file
today <- Sys.Date()
bp.attr("date", format(today, format="%B %d %Y"))
bp.attr("README", "This is just a test.")

# define variables and write data
bp.var("single", 100)
rank = comm.rank()
bp.var("T", (rank+1)*c(1:100))
bp.write()

for(i in 2:6) {
    # append data
    bp.var("single", i*100)
    rank = comm.rank()
    bp.var("T", i*(rank+1)*c(1:100))
    bp.write()
}

bp.flush()

finalize() 