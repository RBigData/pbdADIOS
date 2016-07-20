#include "R_bpls.h"
#include "R_dump.h"






char        filename [256];
int         rank, size, i, block;
int         NX = 100, Global_bounds, Offsets; 
double      t[NX];
uint64_t    adios_groupsize, adios_totalsize;
int64_t       m_adios_group;
int64_t       m_adios_file;
MPI_Comm    comm = MPI_COMM_WORLD;

MPI_Init (&argc, &argv);
MPI_Comm_rank (comm, &rank);
MPI_Comm_size (comm, &size);

Global_bounds = NX * size;
strcpy (filename, "global.bp");

adios_init_noxml (comm);
adios_set_max_buffer_size (20); // Default buffer size for write is 20. User can change this value
adios_declare_group (&m_adios_group, "restart", "iter", adios_flag_yes);
adios_select_method (m_adios_group, "MPI", "", ""); // Default method is MPI. Let users choose different methods later.

// Define variables
adios_define_var (m_adios_group, "NX"
            ,"", adios_integer
            ,0, 0, 0);
adios_define_var (m_adios_group, "Global_bounds"
            ,"", adios_integer
            ,0, 0, 0);
adios_define_var (m_adios_group, "Offsets"
                        ,"", adios_integer
                        ,0, 0, 0);
adios_define_var (m_adios_group, "temperature"
                        ,"", adios_double
                        ,"NX", "Global_bounds", "Offsets");

// Add adios_define_attribute later

adios_open (&m_adios_file, "restart", filename, "w", comm);

adios_groupsize = (4 + 4 + 4 + NX * 8);
adios_group_size (m_adios_file, adios_groupsize, &adios_totalsize);

adios_write(m_adios_file, "NX", (void *) &NX);
adios_write(m_adios_file, "Global_bounds", (void *) &Global_bounds);

Offsets = rank * sub_blocks * NX + block*NX;
adios_write(m_adios_file, "Offsets", (void *) &Offsets);

for (i = 0; i < NX; i++)
    t[i] = Offsets + i;

adios_write(m_adios_file, "temperature", t);

adios_close (m_adios_file);

MPI_Barrier (comm);

adios_finalize (rank);

MPI_Finalize ();