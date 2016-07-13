# load pbdADIOS lib
library(pbdADIOS, quiet = TRUE)

# print variables and attributes
bpls("attributes.bp")

# dump all variables
bp.dump("attributes.bp")

# read a variable
bp.read("attributes.bp", "temperature")

# read a scalar
bp.read("attributes.bp", "NX")

# read a variable with start
bp.read("attributes.bp", "temperature", c(0, 0))

# read a variable with count
bp.read("attributes.bp", "temperature", count=c(1, 2))

# read a variable with start and count
bp.read("attributes.bp", "temperature", c(0, 9), c(1, 1))

# examples with wrong start and count
bp.read("attributes.bp", "temperature", c(0))

bp.read("attributes.bp", "temperature", c(0, 10))

bp.read("attributes.bp", "temperature", count=c(0, 10))

bp.read("attributes.bp", "NX", c(0))

# bp data with multiple steps
heat = bp.dump("heat.bp")
str(heat)

# read only 1st data from the 1st step
bp.read("heat.bp", "T", c(0, 0, 0), c(1, 1, 1))
# read only 1st data from the first two steps
bp.read("heat.bp", "T", c(0, 0, 0), c(2, 1, 1))

#read multiple variables or multiple ranges
bp.read("attributes.bp", list("NX", "temperature", "temperature"))

bp.read("attributes.bp", 
	list("temperature", "temperature"),
	list(c(0,1), c(0, 9)))

bp.read("attributes.bp", 
	list("temperature", "temperature"),
	list(c(0,1), c(0, 9)),
	list(c(1,1), c(1,1)))

bp.read("attributes.bp", 
	list("temperature", "temperature"),
	list(c(0,1), c(0, 9)),
	list(c(1,1)))


finalize() # pbdMPI final

