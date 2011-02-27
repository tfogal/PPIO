#ifndef PARALLEL_POSIX_IO_H
#define PARALLEL_POSIX_IO_H

#include <sys/types.h>

enum ppio_access_mode {
  PPIO_RDONLY,
  PPIO_WRONLY,
  PPIO_RDWR
};

void* open_range(const char* filename, enum ppio_access_mode access,
                 off_t begin, off_t end);
void finished(void* map);

#endif /* PARALLEL_POSIX_IO_H */
