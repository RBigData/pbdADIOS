# custom environment for pbdADIOSS

init_state <- function(envir = .GlobalEnv)
{
    if (!exists(".adiosenv", envir = envir))
        envir$.adiosenv <- new.env(parent = envir)
  
    .adiosenv$nvars <- 0
    .adiosenv$varname_list <- list()
    .adiosenv$var_list <- list()
    .adiosenv$varlength_list <- list()
    .adiosenv$ndim <- list()
    # adios.tag = 0, write; adios.tag = 1, append
    .adiosenv$adios.tag <- 0
    # adios.type = 0, int; adios.type = 1, double
    .adiosenv$adios.type <- list()
  
    invisible()
}
