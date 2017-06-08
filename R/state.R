# custom environment for pbdADIOSS

init_state <- function(envir = .GlobalEnv)
{
    if (!exists(".adiosenv", envir = envir))
        envir$.adiosenv <- new.env(parent = envir)
  
    envir$.adiosenv$nvars <- 0
    envir$.adiosenv$varname_list <- list()
    envir$.adiosenv$var_list <- list()
    envir$.adiosenv$varlength_list <- list()
    envir$.adiosenv$ndim <- list()
    # adios.tag = 0, write; adios.tag = 1, append
    envir$.adiosenv$adios.tag <- 0
    # adios.type = 0, int; adios.type = 1, double
    envir$.adiosenv$adios.type <- list()

    envir$.adiosenv$adios.filename <- 0
    envir$.adiosenv$adios.groupname <- 0
    envir$.adiosenv$adios.group <- 0
  
    invisible()
}