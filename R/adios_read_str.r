adios.selection.boundingbox <- function(adios.ndim, adios.start, adios.count){
    .Call(R_adios_selection_bounding_box, as.integer(adios.ndim),
          as.integer(adios.start), as.integer(adios.count))
}

adios.schedule.read <- function(adios.varinfo, adios.start, adios.count,
                                adios.file.ptr, adios.selection, adios.varname,
                                adios.from.steps, adios.nsteps){
    .Call(R_adios_schedule_read, adios.varinfo, as.integer(adios.start),
          as.integer(adios.count), adios.file.ptr, adios.selection,
          as.character(adios.varname), as.integer(adios.from.steps),
          as.integer(adios.nsteps))
}

custom.data.access <- function(adios.data, adios.selection, adios.varinfo){
    .Call(R_custom_data_access, adios.data, adios.selection,adios.varinfo)
}

adios.perform.reads <- function(adios.file.ptr, adios.blocking){
    .Call(R_adios_perform_reads, adios.file.ptr, as.integer(adios.blocking))
}

adios.advance.step <- function(adios.file.ptr, adios.last, adios.timeout.sec){
    .Call(R_adios_advance_step, adios.file.ptr,
                    as.integer(adios.last), as.numeric(adios.timeout.sec))
    invisible()
}
