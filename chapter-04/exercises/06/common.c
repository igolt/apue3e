
static const char *progname;

/* This feature test macro must be defined to obtain the definitions of
 * SEEK_DATA and SEEK_HOLE from <unistd.h> */
#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define SEEK_ERR()                                                             \
  do {                                                                         \
    fprintf(stderr, "%s: failed to seek: %s\n", progname, strerror(errno));    \
    exit(EXIT_SEEK_ERR);                                                       \
  } while (0)

enum {
  EXIT_INVAL_ARGC = 1,
  EXIT_SRC_OPENERR,
  EXIT_SRC_STATERR,
  EXIT_MEM_ERR,
  EXIT_DST_OPENERR,
  EXIT_SEEK_ERR,
  EXIT_SRC_RDERR,
  EXIT_DST_WRERR,
  EXIT_SRC_CLOSE_ERR,
  EXIT_DST_CLOSE_ERR
};

static off_t seek_or_die(int fd, off_t offset, int whence) {
  offset = lseek(fd, offset, whence);
  if (offset == -1) {
    SEEK_ERR();
  }
  return offset;
}

int cp(const char *src, const char *dst);

int main(int argc, const char *const argv[]) {
  progname = argv[0];

  if (argc != 3) {
    fprintf(stderr, "usage: %s SRC DST\n", progname);
    return EXIT_INVAL_ARGC;
  }

  return cp(argv[1], argv[2]);
}
