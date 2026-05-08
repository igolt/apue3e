#include "common.c"
#include <string.h>

#define _EXIT(_rv)                                                             \
  do {                                                                         \
    rv = _rv;                                                                  \
    goto cleanup;                                                              \
  } while (0)

#define CLOSE(fd) ((fd) == -1 ? 0 : close((fd)))
#define FREE(p) ((p) ? free(p) : ((void)0))

int cp(const char *src, const char *dst) {
  int rv = 0;
  int srcfd = -1, dstfd = -1;
  ssize_t n;
  char *cmpbp = NULL;
  char *bp = NULL;
  struct stat sb;

  srcfd = open(src, O_RDONLY);
  if (srcfd == -1) {
    fprintf(stderr, "%s: failed to open: %s: %s\n", progname, src,
            strerror(errno));
    _EXIT(EXIT_SRC_OPENERR);
  }

  if (fstat(srcfd, &sb) == -1) {
    fprintf(stderr, "%s: failed to get file status: %s: %s\n", progname, src,
            strerror(errno));
    _EXIT(EXIT_SRC_STATERR);
  }

  if ((bp = malloc(sb.st_blksize)) == NULL ||
      (cmpbp = malloc(sb.st_blksize)) == NULL) {
    fprintf(stderr, "%s: failed to allocate memory for copy buffer\n",
            progname);
    _EXIT(EXIT_MEM_ERR);
  }

  memset(cmpbp, 0, sb.st_blksize);

  dstfd = open(dst, O_WRONLY | O_CREAT | O_EXCL, sb.st_mode);
  if (dstfd == -1) {
    fprintf(stderr, "%s: failed to copy: %s: %s\n", progname, dst,
            strerror(errno));
    _EXIT(EXIT_DST_OPENERR);
  }

  /* there shouldn't be any errors since the errors with ftruncate are related
   * with bad file descriptors */
  ftruncate(dstfd, sb.st_size);

  printf("block size: %lu\n", sb.st_blksize);

  while ((n = read(srcfd, bp, sb.st_blksize)) > 0) {
    if (memcmp(cmpbp, bp, sb.st_blksize) == 0) {
      puts("bloco vazio");
      seek_or_die(dstfd, sb.st_blksize, SEEK_CUR);
      continue;
    }

    if (write(dstfd, bp, sb.st_blksize) != n) {
      fprintf(stderr, "%s: failed to create copy: %s: %s\n", progname, dst,
              strerror(errno));
      _EXIT(EXIT_DST_WRERR);
    }
  }

  if (n == -1) {
    fprintf(stderr, "%s: failed reading source file: %s: %s\n", progname, src,
            strerror(errno));
    rv = EXIT_SRC_RDERR;
  }

cleanup:
  if (CLOSE(srcfd)) {
    fprintf(stderr, "%s: failed to close source file: %s: %s\n", progname, src,
            strerror(errno));
    if (rv != 0)
      rv = EXIT_SRC_CLOSE_ERR;
  }

  FREE(bp);
  FREE(cmpbp);

  if (CLOSE(dstfd)) {
    fprintf(stderr, "%s: failed to close dest file: %s: %s\n", progname, dst,
            strerror(errno));
    if (rv != 0)
      rv = EXIT_SRC_CLOSE_ERR;
  }
  return rv;
}
