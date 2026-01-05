/* Figure 1.3 (Page 5): improved the example by implementing a bare-bones
 * `tree(1)`
 */
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#define MAX_LEVEL 255

#define IDENT_SIZE 4

#define streq(s1, s2) (strcmp(s1, s2) == 0)

struct ctx {
  DIR *dp;
  struct dirent *next_dirp;
  size_t path_len;
};

struct dirent *next_dir(DIR *dp) {
  struct dirent *dirp;

  while ((dirp = readdir(dp)) &&
         (streq(dirp->d_name, ".") || streq(dirp->d_name, "..")))
    continue;
  return dirp;
}

int main(int argc, const char *const argv[]) {
  char c;
  int i, j;
  int level, ll;
  size_t path_len;
  const char *initial_path;
  const char *const progname = argv[0];
  struct dirent *dirp, *next_dirp;
  struct stat sb;
  struct ctx ctx[MAX_LEVEL + 1];
  char path[PATH_MAX];

  if (argc > 2) {
    fprintf(stderr, "usage: %s [DIR]\n", progname);
    return EXIT_FAILURE;
  }

  initial_path = argc == 1 ? "." : argv[1];

  /* Initializes path with initial path without trailing '/' */
  path_len = strlen(initial_path);
  while (path_len && initial_path[path_len - 1] == '/')
    --path_len;

  if (path_len + 1 > sizeof(path)) {
    fprintf(stderr, "%s: path too long\n", progname);
    return EXIT_FAILURE;
  }

  strncpy(path, initial_path, path_len);

  /* Initializes the first level context */
  level = ll = 0;
  if ((ctx[level].dp = opendir(initial_path)) == NULL) {
    fprintf(stderr, "%s: failed to open: %s: %s\n", progname, initial_path,
            strerror(errno));
    return EXIT_FAILURE;
  }
  ctx[level].path_len = path_len;
  ctx[level].next_dirp = next_dir(ctx[level].dp);

  /* Prints the tree */
  puts(initial_path);
  do {
    while ((dirp = ctx[level].next_dirp)) {
      for (i = 0; i < ll; i++) {
        for (j = 0; j < IDENT_SIZE; j++)
          putchar(' ');
      }

      for (; i < level + 1; i++) {
        putchar('|');
        c = i == level ? '-' : ' ';
        for (j = 0; j < (IDENT_SIZE - 1); j++)
          putchar(c);
      }

      puts(dirp->d_name);

      if ((ctx[level].next_dirp = next_dir(ctx[level].dp)) == NULL &&
          ll == level)
        ++ll;

      if (level == MAX_LEVEL)
        continue;

      path_len = ctx[level].path_len + strlen(dirp->d_name) + 1;
      if (path_len + 1 > sizeof(path)) {
        fprintf(stderr, "%s: path too long\n", progname);
        continue;
      }

      sprintf(path + ctx[level].path_len, "/%s", dirp->d_name);

      if (lstat(path, &sb) == -1) {
        fprintf(stderr, "%s: failed to get file information: %s: %s\n",
                progname, path, strerror(errno));
        continue;
      }

      if ((sb.st_mode & S_IFMT) != S_IFDIR)
        continue;

      level++;
      if ((ctx[level].dp = opendir(path)) == NULL) {
        fprintf(stderr, "%s: failed to open directory: %s: %s\n", progname,
                path, strerror(errno));
        --level;
        continue;
      }
      ctx[level].path_len = path_len;
      ctx[level].next_dirp = next_dir(ctx[level].dp);
    }
    closedir(ctx[level].dp);
    --level;
  } while (level >= 0);
  return EXIT_SUCCESS;
}
