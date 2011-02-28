#define _POSIX_C_SOURCE 200112L

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/uio.h>

#include "ppio.h"
#include "ppio_internal.h"

/* We'll hint to the kernel to try reading ahead each of a set of buffers, up
 * to this maximum.  The value here is chosen somewhat arbitrarily.  A smarter
 * way to do this might be to look at the memory we've got free and try to fill
 * up 80% of it or something like that.  That implementation is too
 * complicated. */
const size_t MAX_READAHEAD_BUFFERS = 4;

char* readonev(const void* map, const struct ppio_iovec_t *iv, size_t len,
               struct ppio_iovec_t* ov)
{
  void* lkp;
  struct ppio_mapping_t* mapping;
  errno = 0;

  if(len == 0) { errno = EINVAL; return NULL; }
  lkp = p_hash_lookup(map);
  if(lkp == NULL) { errno = EFAULT; return NULL; }

  mapping = (struct ppio_mapping_t*) lkp;

  /* validate all of the input vectors. */
  for(size_t i=0; i < len; ++i) {
    if((iv[i].offset + iv[i].length) > (mapping->offset + mapping->length)) {
      errno = EINVAL;
      return NULL;
    }
  }

  /* We're just going to return the first one.  We would need kernel support to
   * be able to return 'the first buffer that's ready'. */
  char* retval = (mapping->base_address + iv[0].offset);
  ov->offset = iv[0].offset;
  ov->offset = iv[0].length;

  /* Before we bail, though, indicate that a few of the other buffers will be
   * ready soon too. */
  for(size_t i=0; i < len && i < MAX_READAHEAD_BUFFERS; ++i) {
    errno = posix_fadvise(mapping->fd, mapping->offset+iv[i].offset,
                          iv[i].length, POSIX_FADV_WILLNEED);
    assert(errno == 0);
  }

  return retval;
}
