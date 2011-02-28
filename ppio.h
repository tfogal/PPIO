#ifndef PARALLEL_POSIX_IO_H
#define PARALLEL_POSIX_IO_H

#include <sys/types.h>

enum ppio_access_mode {
  PPIO_RDONLY,
  PPIO_WRONLY,
  PPIO_RDWR
};

struct ppio_iovec_t {
  size_t offset;
  size_t length;
};

void* open_range(const char* filename, enum ppio_access_mode access,
                 off_t begin, off_t end);
void finished(void* map);
char* readonev(const void* map, const struct ppio_iovec_t *iv, size_t len,
               struct ppio_iovec_t* ov);

#endif /* PARALLEL_POSIX_IO_H */
