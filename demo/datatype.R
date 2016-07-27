# load pbdADIOS lib
library(pbdADIOS, quiet = TRUE)

#adios_byte
bp.read("write_read_1.bp", "/scalars/scalar_byte") 
#adios_short
bp.read("write_read_1.bp", "/scalars/scalar_short")
#adios_integer
bp.read("write_read_1.bp", "/scalars/scalar_int")
#adios_long
bp.read("write_read_1.bp", "/scalars/scalar_long")

#adios_unsigned_byte
bp.read("write_read_1.bp", "/scalars/scalar_ubyte") 
#adios_unsigned_short
bp.read("write_read_1.bp", "/scalars/scalar_ushort") 
#adios_unsigned_integer
bp.read("write_read_1.bp", "/scalars/scalar_uint")
#adios_unsigned_long
bp.read("write_read_1.bp", "/scalars/scalar_ulong")

#adios_real
bp.read("write_read_1.bp", "/scalars/scalar_float")
#adios_double
bp.read("write_read_1.bp", "/scalars/scalar_double")

#adios_string
bp.read("write_read_1.bp", "/scalars/scalar_string")

#adios_complex
bp.read("write_read_1.bp", "/scalars/scalar_complex")
#adios_double_complex
bp.read("write_read_1.bp", "/scalars/scalar_double_complex")

finalize() # pbdMPI final



              