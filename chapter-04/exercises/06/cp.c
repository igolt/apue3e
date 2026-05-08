/* Write a utility like cp(1) that copies a file containing holes, without
writing the bytes of 0 to the output file. */

#include "common.c"

#define _EXIT(_rv)                                                             \
  do {                                                                         \
    rv = _rv;                                                                  \
    goto cleanup;                                                              \
  } while (0)

#define CLOSE(fd) ((fd) == -1 ? 0 : close((fd)))

int cp(const char *src, const char *dst) {
  int rv;
  int srcfd = -1, dstfd = -1;
  char buffer[4096];
  ssize_t n;
  off_t offset, dbegin, dend;
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

  dstfd = open(dst, O_WRONLY | O_CREAT | O_EXCL, sb.st_mode);
  if (dstfd == -1) {
    fprintf(stderr, "%s: failed to copy: %s: %s\n", progname, dst,
            strerror(errno));
    _EXIT(EXIT_DST_OPENERR);
  }

  /* there shouldn't be any errors since the errors with ftruncate are related
   * with bad file descriptors */
  ftruncate(dstfd, sb.st_size);

  /* SEEK_DATA and SEEK_HOLE extensions are used to find holes in the file.
   * Those are extensions and are not garanteed to be supported by every *NIX
   * system
   */
  dend = offset = 0;
  while (dend < sb.st_size) {
    dbegin = lseek(srcfd, dend, SEEK_DATA);
    if (dbegin == -1) {
      if (errno == ENXIO)
        break;

      SEEK_ERR();
    }

    dend = seek_or_die(srcfd, dbegin, SEEK_HOLE);

    if (offset != dbegin) {
      offset = dbegin;
      seek_or_die(dstfd, offset, SEEK_SET);
    }

    seek_or_die(srcfd, offset, SEEK_SET);

    while (offset < dend) {
      n = MIN(sizeof(buffer), (size_t)(dend - offset));
      n = read(srcfd, buffer, n);
      if (n == -1) {
        fprintf(stderr, "%s: failed reading source file: %s: %s\n", progname,
                src, strerror(errno));
        _EXIT(EXIT_SRC_RDERR);
      }

      offset += n;

      if (write(dstfd, buffer, n) != n) {
        fprintf(stderr, "%s: failed to create copy: %s: %s\n", progname, dst,
                strerror(errno));
        _EXIT(EXIT_DST_WRERR);
      }
    }
  }

cleanup:
  if (CLOSE(srcfd)) {
    fprintf(stderr, "%s: failed to close source file: %s: %s\n", progname, src,
            strerror(errno));
    rv = EXIT_SRC_CLOSE_ERR;
  }

  if (CLOSE(dstfd)) {
    fprintf(stderr, "%s: failed to close dest file: %s: %s\n", progname, dst,
            strerror(errno));
    rv = EXIT_DST_CLOSE_ERR;
  }
  return rv;
}
