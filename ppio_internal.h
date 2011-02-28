#ifndef PARALLEL_POSIX_IO_INTERNAL_H
#define PARALLEL_POSIX_IO_INTERNAL_H

#include <stdlib.h>
#include <sys/types.h>

/* Global hash table for mapping... anything to anything.
 * We use it to map mmap'd regions to file descriptors. */
#define HTYPE(x) (void*)((size_t)x)
void p_hash_add(const void* from, void* to);
void p_hash_remove(const void* from);
void* p_hash_lookup(const void* from);

struct ppio_mapping_t {
  int fd; /* file that's open for this. */
  void* base_address;
  off_t offset; /* where this mapping started in the file. */
  size_t length; /* how many bytes we've mapped */
};

#endif /* PARALLEL_POSIX_IO_INTERNAL_H */
