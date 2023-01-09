//memory management header

#include "kernel/types.h"
#include "user/user.h"

struct header {
  struct header *next;
  uint size;
  //1 if it is allocated
  //-1 if it is not allocated
  int is_allocated;
};



void _free(void *ap);
void* _malloc(uint nbytes);
