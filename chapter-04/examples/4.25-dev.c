#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

int main(int argc, const char *const argv[]) {
  const char *const progname = argv[0];
  struct stat sb;

  if (argc == 1) {
    fprintf(stderr, "usage: %s FILE...\n", progname);
    return 1;
  }

  while (--argc) {
    if (stat(*++argv, &sb) == -1) {
      fprintf(stderr, "%s: failed to stat: %s: %s\n", progname, *argv,
              strerror(errno));
      continue;
    }

    printf("%s:\n", *argv);
    printf("%d/%d\n", major(sb.st_dev), minor(sb.st_dev));
    if (S_ISCHR(sb.st_mode) || S_ISBLK(sb.st_mode)) {
      printf("%s rdev %d/%d\n", S_ISCHR(sb.st_mode) ? "character" : "block",
             major(sb.st_rdev), minor(sb.st_rdev));
    }

    putchar('\n');
  }

  return 0;
}
