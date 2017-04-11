#ifndef tdmem_h___
#define tdmem_h___

#include "../platabs/tcomdef.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define TDM_MALLOCVAPI  extern



extern 		void *	Malloc(TDUINT size);
//TDM_MALLOCVAPI
void *	Calloc(TDUINT nelem, TDUINT elsize);
//TDM_MALLOCVAPI
void *	Realloc(void *ptr, TDUINT size);
extern void		TDFree(void *ptr);


#endif /* tdmem_h___ */



#ifdef __cplusplus
}
#endif
