/* Generate a sparse file based on the format specifier. Format specifier has
 * the following format:
 *
 * [n]section
 *
 * Where:
 *  n:
 *   number between in the range [1, 8], which is the size in KB of the next
 * section
 *
 *  section:
 *   d -> write data (consecutive 'a's)
 *   h -> hole
 *
 *  e.g the following format d4h8d would generate a file with 1K data, 4K hole
 * and 8K data.
 */

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define K 1024

#define DEFAULT_FNAME "sparsefile"

#define SIZE_MIN 1
#define SIZE_MAX 8

#define MAKE_HOLE()                                                            \
  do {                                                                         \
    off_t __pos;                                                               \
    size *= K;                                                                 \
    if ((__pos = lseek(fd, size, SEEK_CUR)) == -1 ||                           \
        ftruncate(fd, __pos) == -1) {                                          \
      fprintf(stderr, "%s: failed to make hole: %s: %s\n", progname, fname,    \
              strerror(errno));                                                \
      return 1;                                                                \
    }                                                                          \
  } while (0)

#define WRITE_DATA()                                                           \
  do {                                                                         \
    size *= K;                                                                 \
    if (write(fd, buffer, size) != size) {                                     \
      fprintf(stderr, "%s: failed to write data: %s\n", progname,              \
              strerror(errno));                                                \
      return 1;                                                                \
    }                                                                          \
  } while (0)

int main(int argc, const char *const argv[]) {
  int fd;
  ssize_t size;
  const char *fname;
  const char *format;
  const char *const progname = argv[0];
  char buffer[8 * K];

  if (argc < 2 || argc > 3) {
    fprintf(stderr, "usage: %s [FNAME] FORMAT\n", progname);
    return 1;
  }

  fname = argc == 2 ? DEFAULT_FNAME : argv[1];
  format = argc == 2 ? argv[1] : argv[2];

  fd = creat(fname, 0644);
  if (fd == -1) {
    fprintf(stderr, "%s: failed to create file: %s: %s\n", progname, fname,
            strerror(errno));
    return 1;
  }

  memset(buffer, 'a', sizeof(buffer));

  /* Just wanted to use goto (don't known why) */

loop_begin: {
  size = 1;
loop_begin_after_set_size:
  switch (*format) {
  case '\0':
    goto loop_end;
    break;

  case 'h':
    MAKE_HOLE();
    break;

  case 'd':
    WRITE_DATA();
    break;

  default:
    if (!isdigit(*format)) {
      fprintf(stderr, "%s: invalid format specifier\n", progname);
      return 1;
    }

    errno = 0;
    size = strtol(format, (char **)&format, 10);
    if (errno) {
      fprintf(stderr, "%s: failed to parse format specifier: %s\n", progname,
              strerror(errno));
      return 1;
    }

    if (size < SIZE_MIN || size > SIZE_MAX) {
      fprintf(stderr, "%s: size should be in range [%u, %u], got %lu\n",
              progname, SIZE_MIN, SIZE_MAX, size);
      return 1;
    }

    goto loop_begin_after_set_size;
  }
  ++format;
  goto loop_begin;
}
loop_end:

  if (close(fd)) {
    fprintf(stderr, "%s: WARNING: failed to close file: %s: %s\n", progname,
            fname, strerror(errno));
    return 1;
  }

  return 0;
}
