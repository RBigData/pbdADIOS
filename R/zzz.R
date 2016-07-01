# Hooks for Name Space events

# triggered by library()
.onLoad <- function(libname, pkgname)
{
    pbdMPI::init() # pbdMPI initilization
    if(.Platform$OS.type == "windows")  print("windows")
    else print("others")
} 

 
# triggered by detach(2, unload=TRUE)
.onUnload <- function(libpath)
{
    pbdMPI::finalize()
    print("Bye!")
} 

# can set some global env variables here