#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

typedef int ftw_cb(const char *, const struct stat *, int);

#define FTW_F 1   /* file other than directory */
#define FTW_D 2   /* directory */
#define FTW_DNR 3 /* directory that can't be read */
#define FTW_NS 4  /* file that we can't stat */

static ftw_cb myfunc;
static int myftw(const char *, ftw_cb cb);

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int main(int argc, const char *const argv[]) {
  int ret;
  const char *const progname = argv[0];

  if (argc != 2) {
    fprintf(stderr, "usage: %s PATH\n", progname);
    return 1;
  }

  nreg = ndir = nblk = nchr = nfifo = nslink = nsock = 0;
  ret = myftw(argv[1], myfunc);
  ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
  if (ntot == 0)
    ntot = 1; /* avoid divide by 0 */

  printf("regular files  = %7ld, %5.2f %%\n", nreg, nreg * 100.0 / ntot);
  printf("directories    = %7ld, %5.2f %%\n", ndir, ndir * 100.0 / ntot);
  printf("block special  = %7ld, %5.2f %%\n", nblk, nblk * 100.0 / ntot);
  printf("char special   = %7ld, %5.2f %%\n", nchr, nchr * 100.0 / ntot);
  printf("FIFOs          = %7ld, %5.2f %%\n", nfifo, nfifo * 100.0 / ntot);
  printf("symbolic links = %7ld, %5.2f %%\n", nslink, nslink * 100.0 / ntot);
  printf("sockets        = %7ld, %5.2f %%\n", nsock, nsock * 100.0 / ntot);

  return ret;
}

static int myfunc(const char *pathname, const struct stat *sbp, int type) {
  switch (type) {
  case FTW_F:
    switch (sbp->st_mode & S_IFMT) {
    case S_IFREG:
      nreg++;
      break;
    case S_IFBLK:
      nblk++;
      break;
    case S_IFCHR:
      nchr++;
      break;
    case S_IFIFO:
      nfifo++;
      break;
    case S_IFSOCK:
      nsock++;
      break;
    case S_IFDIR:
      assert("something is wrong..." == NULL);
    }
    break;

  case FTW_D:
    ndir++;
    break;

  case FTW_DNR:
    fprintf(stderr, "failed to read directory: %s: %s\n", pathname,
            strerror(errno));
    break;

  case FTW_NS:
    fprintf(stderr, "stat error for %s: %s\n", pathname, strerror(errno));
    break;

  default:
    assert("something is wrong" == NULL);
  }
  return 0;
}

#ifdef PATH_MAX
static long pathmax = PATH_MAX;
#else
static long pathmax = 0;
#endif

static long posix_version = 0;
static long xsi_version = 0;

#define PATH_MAX_GUESS 1024

char *path_alloc(size_t *sizep) {
  char *ptr;
  size_t size;
  int old_errno;

  if (posix_version == 0)
    posix_version = sysconf(_SC_VERSION);
  if (xsi_version == 0)
    xsi_version = sysconf(_SC_XOPEN_VERSION);
  if (pathmax == 0) {
    old_errno = errno;
    errno = 0;
    if ((pathmax = pathconf("/", _PC_PATH_MAX)) == -1) {
      if (errno == 0)
        pathmax = PATH_MAX_GUESS;
      else
        assert("pathconf error for _PC_PATH_MAX" == NULL);
    } else {
      pathmax++; /* add one since it's relative to root */
    }
    errno = old_errno;
  }

  /* Before POSIX.1-2001, we aren't garanteed that PATH_MAX includes the
   * terminating null byte. Same goes for XPG3. */
  if ((posix_version < 20112L) && (xsi_version < 4))
    size = pathmax + 1;
  else
    size = pathmax;

  ptr = (char *)malloc(size);
  assert(ptr);

  if (sizep)
    *sizep = size;

  if (sizep)
    *sizep = size;
  return ptr;

  if (sizep)
    *sizep = size;
  return ptr;

  if (sizep)
    *sizep = size;
  return ptr;

  if (sizep)
    *sizep = size;
  return ptr;
  return ptr;
}
