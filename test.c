#define _POSIX_C_SOURCE 200112L
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <check.h>

#include "ppio.h"

#define error(x) \
  do { \
    fprintf(stderr, "%s\n", x); \
    exit(EXIT_FAILURE); \
  } while(0)

static void full_read(int fd, char* start, ssize_t len)
{
  ssize_t bytes=0;
  if(len <= 0) {
    error("Invalid number of bytes.");
  }

  assert(len <= SSIZE_MAX);

  do {
    ssize_t res = read(fd, start+bytes, len-bytes);
    if(res == -1) {
      perror("read");
      error("read failed");
    }
    if(res == 0) {
      error("file too small!");
    }
    bytes += res;
  } while(bytes < len);
}

static void full_write(int fd, const void* buf, size_t bytes)
{
  size_t offset=0;
  if(bytes == 0) {
    error("asked to write 0 bytes...");
  }
  do {
    ssize_t res = write(fd, ((const char*)buf) + offset, bytes-offset);
    if(res == -1) {
      perror("write");
      error("write failed.");
    }
    if(res == 0) {
      fprintf(stderr, "wrote 0 bytes...\n");
    }
    offset += res;
  } while(offset < bytes);
}

/* the return variable must be freed! */
static char* random_data(size_t len)
{
  int fd;
  void* buf;

  if(posix_memalign(&buf, 4096, len) != 0) {
    error("allocation failed");
  }

  if((fd = open("/dev/urandom", O_RDONLY)) == -1) {
    error("open of /dev/urandom failed.");
  }
  full_read(fd, (char*)buf, len);

  close(fd);

  return buf;
}

static const char* testfile = ".ppio-test-data";
/* create a large file we can mmap. */
void setup()
{
  int fd;
  char* data = random_data(16384);

  fd = open(testfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR);
  if(fd < 0) {
    perror("open");
    error("could not create test data file");
  }
  for(size_t i=0; i < 16; ++i) {
    full_write(fd, data, 16384);
  }
  free(data);
}

void teardown()
{
  if(unlink(testfile) != 0) {
    perror("unlinking test data file");
  }
}

START_TEST(test_nothing)
{
}
END_TEST

START_TEST(test_simple)
{
  void* range = open_range(testfile, PPIO_RDONLY, 0, 42);
  fail_if(errno != 0, "open_range set errno");
  fail_if(range == NULL, "Could not map test data file!");
  finished(range);
  fail_if(errno != 0, "finished set errno to %d", errno);
}
END_TEST

START_TEST(test_invalid_number_of_bytes)
{
  void* range = open_range(testfile, PPIO_RDONLY, 0, 0);
  fail_unless(errno != 0, "open_range should have set errno.");
  fail_unless(range == NULL, "open_range should have failed.");
}
END_TEST

START_TEST(test_invalid_finished_addr)
{
  void* garbage = (void*)0xdeadbeef;
  finished(garbage);
  fail_unless(errno == EFAULT, "finished did not detect a bad address");
}
END_TEST

Suite* ppio_suite()
{
  Suite* s = suite_create("PPIO");
  TCase* core = tcase_create("Core");
  tcase_add_test(core, test_nothing);
  tcase_add_test(core, test_simple);
  tcase_add_test(core, test_invalid_number_of_bytes);
  tcase_add_test(core, test_invalid_finished_addr);
  tcase_add_checked_fixture(core, setup, teardown);

  suite_add_tcase(s, core);
  return s;
}

int main()
{
  int failed;
  Suite* s = ppio_suite();
  SRunner* sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return failed;
}
