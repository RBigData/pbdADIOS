#ifndef PBDADIOS_FINALIZER
#define PBDADIOS_FINALIZER

#include "R_adios.h"

/**
 *  Finalizer that frees memory and clears R pointer
 */
static inline void finalizer(SEXP Rptr)
{
  void *ptr = (void *) R_ExternalPtrAddr(Rptr);
  if (NULL == ptr)
  {
    R_debug_print("finalizer: Nothing to finalize\n");
    return;
  }
  else
  {
    R_debug_print("finalizer: About to free: %p ...\n", ptr);
    Free(ptr);
    R_debug_print("finalizer: Freed %p.\n", ptr);
    R_ClearExternalPtr(Rptr);
    R_debug_print("finalizer: %p Cleared Rptr.\n", ptr);
  }
}



/** 
 *  Finalizer that only clears R pointer
 */
static inline void finalizer0(SEXP Rptr)
{
  void *ptr = (void *) R_ExternalPtrAddr(Rptr);
  if (NULL == ptr)
  {
    R_debug_print("finalizer0: Nothing to finalize\n");
    return;
  }
  else
  {
    R_debug_print("finalizer0: Freed by ADIOS %p. Only clear.\n", ptr);
    R_ClearExternalPtr(Rptr);
    R_debug_print("finalizer0: %p Cleared Rptr.\n", ptr);
  }
}


#endif
