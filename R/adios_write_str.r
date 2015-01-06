#' @title Write Initialization
#' 
#' @description 
#' A description of what the function does.
#' 
#' @param adios.init.noxml 
#' A communicator number.
#' Description of the variable.
#' 
#' @details
#' A more detailed description, if you like.
#' 
#' @return Description of the return.
#' 
#' @seealso \code{\link{adios.open}}
#' 
#' @examples \dontrun{
#' library(pbdADIOS)
#' whatever <- adios.init.noxml(stuff)
#' }
#' 
#' @export

adios.init.noxml <- function(comm = pbdMPI::.SPMD.CT$comm){
  .Call("R_adios_init_noxml",comm.c2f(comm))	 
  invisible()
}

adios.allocate.buffer <- function(adios.buffersize){
  .Call("R_adios_allocate_buffer",as.integer(adios.buffersize))	
  invisible()
} 

adios.declare.group <- function(adios.groupname, adios.timeindex){
  .Call("R_adios_declare_group", as.character(adios.groupname), as.character(adios.timeindex))
} 

adios.select.method <- function(adios.group_p, adios.method, adios.params, adios.basepath){
		    
  .Call("R_adios_select_method", adios.group_p, as.character(adios.method), as.character(adios.params), as.character(adios.basepath))		    
  invisible()
}

adios.define.var <- function(adios.group_p, adios.varname, adios.path, adios.localdim, adios.globaldim, adios.localoffset ){
  
  .Call("R_adios_define_var",adios.group_p, as.character(adios.varname), as.character(adios.path), as.character(adios.localdim), as.character(adios.globaldim), as.character(adios.localoffset))
  invisible()
}

adios.open <- function(adios.groupname, adios.filename, adios.mode, comm = pbdMPI::.SPMD.CT$comm){
	
  .Call("R_adios_open",as.character(adios.groupname), as.character(adios.filename), as.character(adios.mode),comm.c2f(comm))
}

adios.group.size <- function(adios.file_p, adios.groupsize){
  .Call("R_adios_group_size", adios.file_p, as.integer(adios.groupsize))
 }

adios.write <- function(adios.file_p, adios.varname, adios.varaddress){
  .Call("R_adios_write",adios.file_p, as.character(adios.varname), adios.varaddress)
}

adios.close <- function(adios.file_p){
  .Call("R_adios_close", adios.file_p)
   invisible()
}

adios.finalize <- function(comm = .SPMD.CT$comm){
  .Call("R_adios_finalize", comm.rank(comm))
  invisible()
} # End of adios.finalize()




