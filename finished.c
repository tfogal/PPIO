#define _POSIX_C_SOURCE 200112L
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>
#include "ppio.h"
#include "ppio_internal.h"

void finished(void* map)
{
  void* lkp = p_hash_lookup(map);
  struct ppio_mapping_t* mapping;
  errno = 0;
  if(lkp == NULL) {
    errno = EFAULT;
    return;
  }

  mapping = (struct ppio_mapping_t*) lkp;
  if(munmap(mapping->base_address, mapping->length) != 0) {
    /* We want to close our file.  But we want errno to be the real cause of
     * the problem -- what happened to make this munmap fail -- so we save and
     * restore it so that close(2) won't clobber our errno value. */
    int saved_errno = errno;
    close(mapping->fd);
    errno = saved_errno;
    return;
  }
  close(mapping->fd);
  free(mapping);
  p_hash_remove(map);
}
