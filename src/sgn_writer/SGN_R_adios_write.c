/* 
 * ADIOS is freely available under the terms of the BSD license described
 * in the COPYING file in the top level directory of this source distribution.
 *
 * Copyright (c) 2008 - 2009.  UT-BATTELLE, LLC. All rights reserved.
 */

/* ADIOS C Example: write a global array from N processors with gwrite
 *
 * How to run: mpirun -np <N> adios_global
 * Output: adios_global.bp
 * ADIOS config file: adios_global.xml
 *
*/

#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include "adios.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

typedef struct {
  int64_t       m_adios_group;
  int64_t       m_adios_file;
} R_adios_file_group;

R_adios_file_group * Radios_write_open(MPI_Comm, char *,char *,char *,char *); 
            //comm, groupname, transportmethod, filename, mode  : Return "R_adios_file_group" struct

int Radios_write_close(MPI_Comm, int, char **, int *, char **, char **, char **, R_adios_file_group *, void **);


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

R_adios_file_group * Radios_write_open(MPI_Comm comm,char *group_name,char *transport_method,char *filename,char *mode)
{
    adios_init_noxml (comm); 
    adios_allocate_buffer (ADIOS_BUFFER_ALLOC_NOW, 10); 
    
    R_adios_file_group  *R_adios_info;
    R_adios_info = (R_adios_file_group*) malloc(sizeof(R_adios_file_group));
    
    adios_declare_group (&(R_adios_info->m_adios_group), group_name, "", adios_flag_yes);
    adios_select_method (R_adios_info->m_adios_group,transport_method, "", ""); 

    adios_open(&(R_adios_info -> m_adios_file), group_name, filename, mode, comm); // does it need to open multiple time 

    return R_adios_info;
}

//int Radios_write(char *variables[], int totalvars){
///
//   printf("var[1]=%s",(char* )variables[3]);
//
//
//}


int Radios_write_close(MPI_Comm comm, int numvars, char **varnames, int  *type, char **local_dim, char **global_dim, char **local_offset, R_adios_file_group *R_adios_info, void **data){

 for(int i=0;i<numvars;i++){
	
    adios_define_var (R_adios_info->m_adios_group, varnames[i]
			    ,"", type[i], local_dim[i], global_dim[i], local_offset[i]);
  }

  uint64_t adios_groupsize, adios_totalsize;
  //adios_groupsize = 4 + 4 + 4 + 20 * 8 ; //Hard coded for now

  adios_groupsize = 0;
  for(int i=0;i<numvars;i++){
    int size=0;

    //Add extra types based on ADIOS_DATATYPES 
    if(type[i]==2) {size=4;}
    else if(type[i]==6) {size=8;}
    else{} //look for manual

    if(local_dim[i]==0){adios_groupsize += size;}
    else{

      for(int j=0;j<numvars;j++){
	if(strcmp(local_dim[i],varnames[j])==0){
	  adios_groupsize += size * (*(int*)(data[j]));   
	  //printf("value %lf",(*(int*)(data[j])));
	   break;
	}
	
      }

    }

  }
  //printf("size of group is %d \n",adios_groupsize);
 //adios_groupsize = 4 + 4 + 4 + NX * 8 ; 
                    //+ 4 + 4 + 4 + NX * 8;

  adios_group_size (R_adios_info->m_adios_file, adios_groupsize, &adios_totalsize);
  
  for(int j=0;j<numvars;j++){
    adios_write(R_adios_info->m_adios_file, varnames[j], (void *) data[j]);

  }

  adios_close (R_adios_info->m_adios_file);
  MPI_Barrier (comm);

  return 0;
}


