#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "R_adios.h"
#include "adios_read.h"


void adios_file_finalize(SEXP ptr)
{
  if (NULL == R_ExternalPtrAddr(ptr)) return;
  
  ADIOS_FILE *fp = (ADIOS_FILE *) R_ExternalPtrAddr(ptr);
  free(fp);
  R_ClearExternalPtr(ptr);
}



void adios_varinfo_finalize(SEXP ptr)
{
  if (NULL == R_ExternalPtrAddr(ptr)) return;
  
  ADIOS_VARINFO *info = (ADIOS_VARINFO *) R_ExternalPtrAddr(ptr);
  free(info);
  R_ClearExternalPtr(ptr);
}



void adios_selection_finalize(SEXP ptr)
{
  if (NULL == R_ExternalPtrAddr(ptr)) return;
  
  ADIOS_SELECTION *selection = (ADIOS_SELECTION *) R_ExternalPtrAddr(ptr);
  free(selection);
  R_ClearExternalPtr(ptr);
}



void adios_data_finalize(SEXP ptr)
{
  if (NULL == R_ExternalPtrAddr(ptr)) return;
  
  void *data = (void *) R_ExternalPtrAddr(ptr);
  free(data);
  R_ClearExternalPtr(ptr);
}

