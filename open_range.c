#define _POSIX_C_SOURCE 200112L
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "ppio.h"
#include "ppio_internal.h"

void* open_range(const char* filename, enum ppio_access_mode access,
                 off_t begin, off_t end)
{
  int fd;
  int acc;
  int mmap_prot, mmap_flags;
  void* map;
  errno = 0;

  switch(access) {
    case PPIO_RDONLY:
      acc = O_RDONLY;
      mmap_prot = PROT_READ;
      mmap_flags = MAP_PRIVATE;
      break;
    case PPIO_WRONLY:
      acc = O_WRONLY;
      mmap_prot = PROT_WRITE;
      mmap_flags = MAP_SHARED;
      break;
    case PPIO_RDWR:
      acc = O_RDWR;
      mmap_prot = PROT_READ | PROT_WRITE;
      mmap_flags = MAP_SHARED;
      break;
  }

  fd = open(filename, acc);
  if(fd == -1) {
    return NULL;
  }

  map = mmap(NULL, end-begin, mmap_prot, mmap_flags, fd, begin);
  if(map == MAP_FAILED) {
    /* We don't want close(2) to clobber our errno. */
    int saved_errno = errno;
    close(fd);
    errno = saved_errno;
    return NULL;
  }

  struct ppio_mapping_t* mapping = calloc(1, sizeof(struct ppio_mapping_t));
  if(mapping == NULL) {
    munmap(map, end-begin);
    close(fd);
    errno = ENOMEM;
    return NULL;
  }
  mapping->fd = fd;
  mapping->base_address = map;
  mapping->offset = begin;
  mapping->length = end-begin;
  p_hash_add(map, mapping);
  return map;
}
