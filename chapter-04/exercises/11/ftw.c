/* In Section 4.22, our version of ftw never changes its directory. Modify this
routine so that each time it encounters a directory, it uses the chdir function
to change to that directory, allowing it to use the filename and not the
pathname for each call to lstat. When all the entries in a directory have been
processed, execute chdir(".."). Compare the time used by this version and the
version in the text. */

#include "common.h"

#include <stdio.h>

static int dopath(const char *, ftw_cb);

static char *fullpath;
static size_t pathlen;

static int myftw(const char *pathname, ftw_cb cb) {
  size_t pathname_len;

  fullpath = path_alloc(&pathlen);

  pathname_len = strlen(pathname);
  if (pathlen <= pathname_len) {
    pathlen = pathname_len << 1;
    fullpath = realloc(fullpath, pathlen);

    assert(fullpath);
  }

  strcpy(fullpath, pathname);
  return dopath(pathname, cb);
}

#define xchdir(path)                                                           \
  do {                                                                         \
    if (chdir((path)) == -1) {                                                 \
      fprintf(stderr, "failed to chdir: %s: %s\n", (path), strerror(errno));   \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

static int dopath(const char *pathname, ftw_cb cb) {
  struct stat sb;
  struct dirent *dirp;
  DIR *dp;
  int ret, n;

  if (lstat(pathname, &sb) == -1)
    return cb(fullpath, &sb, FTW_NS);

  if (!S_ISDIR(sb.st_mode))
    return cb(fullpath, &sb, FTW_F);

  if ((ret = cb(fullpath, &sb, FTW_D)))
    return ret;

  n = strlen(fullpath);
  if (n + NAME_MAX + 2 > pathlen) {
    pathlen <<= 1;
    fullpath = realloc(fullpath, pathlen);
    assert(fullpath);
  }
  fullpath[n++] = '/';
  fullpath[n] = 0;

  if ((dp = opendir(pathname)) == NULL)
    return cb(fullpath, &sb, FTW_DNR);

  xchdir(pathname);

  while ((dirp = readdir(dp))) {
    if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
      continue;
    strcpy(&fullpath[n], dirp->d_name);
    if ((ret = dopath(dirp->d_name, cb)))
      break;
  }

  xchdir("..");

  fullpath[n - 1] = 0;
  if (closedir(dp))
    fprintf(stderr, "can't close directory %s", fullpath);
  return ret;
}
