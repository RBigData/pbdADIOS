# load pbdADIOS lib
library(pbdADIOS, quiet = TRUE)

bp.create("bptest.bp")

bp.var("single", 100)

rank = comm.rank()

bp.var("T", (rank+1)*c(1:100))

today <- Sys.Date()

bp.attr("date", format(today, format="%B %d %Y"))
bp.attr("README", "This is just a test.")

bp.write()

finalize() 