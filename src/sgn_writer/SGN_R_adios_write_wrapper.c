/* 
 * How to run: mpirun -np <N> adios_global
 * Output: adios_global_test.bp
 *
*/

#include "../R_adios.h"

R_adios_file_group * Radios_write_open(MPI_Comm comm,char *group_name,char *transport_method,char *filename,char *mode)
{
    adios_init_noxml (comm); 
    adios_allocate_buffer (ADIOS_BUFFER_ALLOC_NOW, 10); 
    
    R_adios_file_group  *R_adios_info;
    R_adios_info = (R_adios_file_group*) malloc(sizeof(R_adios_file_group));
    
    adios_declare_group (&(R_adios_info->m_adios_group), group_name, "", adios_flag_yes);
    adios_select_method (R_adios_info->m_adios_group,transport_method, "", ""); 

    adios_open(&(R_adios_info -> m_adios_file), group_name, filename, mode, comm); 

    return R_adios_info;
}

int Radios_write_close(MPI_Comm comm, int numvars, char **varnames, int  *type, char **local_dim, char **global_dim, char **local_offset, R_adios_file_group *R_adios_info, void **data){

 for(int i=0;i<numvars;i++){
    adios_define_var (R_adios_info->m_adios_group, varnames[i]
			    ,"", type[i], local_dim[i], global_dim[i], local_offset[i]);
  }

  uint64_t adios_groupsize, adios_totalsize;

          //adios_groupsize = 4 + 4 + 4 + 20 * 8 ; //Hard coded for now

  //Calculate Group Size
  adios_groupsize = 0;
  for(int i=0;i<numvars;i++){
    int size=0;

    //TODO : Add extra types based on ADIOS_DATATYPES

    if(type[i]==2) {size=4;} // INTEGER
    else if(type[i]==6) {size=8;} // DOUBLE
    else if(type[i]==5) {size=4;} // REAL
    else if(type[i]==10) {size=8;} // COMPLEX
    else if(type[i]==4) {size=8;} // LONG
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

  adios_group_size (R_adios_info->m_adios_file, adios_groupsize, &adios_totalsize);
  
  for(int j=0;j<numvars;j++){
    adios_write(R_adios_info->m_adios_file, varnames[j], (void *) data[j]);
  }

  adios_close (R_adios_info->m_adios_file);
  MPI_Barrier (comm);

  return 0;
}


SEXP R_adios_finalize(SEXP R_comm_rank){
  adios_finalize(INTEGER(R_comm_rank)[0]);
  return(R_NilValue);
} /* End of R_adios_finalize(). */



