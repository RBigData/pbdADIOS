/* 
 * How to run: mpirun -np <N> adios_global
 * Output: adios_global_test.bp
 *
*/
//#include <stdio.h>
//#include <string.h>
//#include "mpi.h"
#include "../R_adios.h"
//#include "adios.h"

int main (int argc, char ** argv) 
{

  //############### 

    //Data creation
    int numvars = 4;
    char *varnames[4];
    varnames[0] = "NX";
    varnames[1] = "G";
    varnames[2] = "O";
    varnames[3] = "temperature";
    //varnames = malloc(sizeof(char *) *numvars );                                        
  
    int type[4] = {2,2,2,6}; //Ask Norbert. ADIOS_DATATYPES is in manual                 
                               
    char *local_dim[]={{0},{0},{0},{"NX"}};
    char *global_dim[]={{0},{0},{0},{"G"}};
    char *local_offset[]={{0},{0},{0},{"O"}};

    int NX = 20;
    double  t[NX];
    int O;
  

    int rank, size;
    MPI_Comm    comm = MPI_COMM_WORLD;
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (comm, &rank);
    MPI_Comm_size (comm, &size);
    
    //Do we want to write "O" and "temperature" within one step (twice)                   
    for (int i = 0; i < NX; i++) {
      t[i] = rank + i*0.1 + 0.01;
    }

    int G =  NX * size;

    //###############     


     // 5 iter
     for (int j =0; j < 5; j++){
       
       R_adios_file_group *R_adios_info = Radios_write_open(comm,"restart","MPI","adios_globaltime_test.bp","a");

       O = rank * 2 * NX + NX; //depends on rank 
       void *data[] =  {&NX, &G, &O, t};

       //hack figuring out type
       //varname, type, 

       Radios_write_close(comm, numvars, varnames, type, local_dim, global_dim, local_offset, R_adios_info, data);


     } // iter for loop over

     adios_finalize (rank);
     MPI_Finalize ();

     return 0;
}


