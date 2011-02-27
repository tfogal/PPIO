#ifndef PARALLEL_POSIX_IO_INTERNAL_H
#define PARALLEL_POSIX_IO_INTERNAL_H

#include <stdlib.h>

/* Global hash table for mapping... anything to anything.
 * We use it to map mmap'd regions to file descriptors. */
#define HTYPE(x) (void*)((size_t)x)
void p_hash_add(const void* from, void* to);
void p_hash_remove(const void* from);
void* p_hash_lookup(const void* from);

struct ppio_mapping_t {
  int fd;
  void* base_address;
  size_t length;
};

#endif /* PARALLEL_POSIX_IO_INTERNAL_H */
