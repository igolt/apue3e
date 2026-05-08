#include "common.h"

static int dopath(ftw_cb);

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
  return dopath(cb);
}

static int dopath(ftw_cb cb) {
  struct stat sb;
  struct dirent *dirp;
  DIR *dp;
  int ret, n;

  if (lstat(fullpath, &sb) == -1)
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

  if ((dp = opendir(fullpath)) == NULL)
    return cb(fullpath, &sb, FTW_DNR);

  while ((dirp = readdir(dp))) {
    if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
      continue;
    strcpy(&fullpath[n], dirp->d_name);
    if ((ret = dopath(cb)))
      break;
  }

  fullpath[n - 1] = 0;
  if (closedir(dp))
    fprintf(stderr, "can't close directory %s", fullpath);
  return ret;
}
