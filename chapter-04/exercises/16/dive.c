#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define DIR_NAME "0123456789abcdef"
#define DIR_NAME_LEN (sizeof(DIR_NAME) - 1)

char *buffer = NULL;
size_t capacity = 0;

void grow_buffer(size_t new_capacity) {
  capacity = new_capacity;
  buffer = realloc(buffer, new_capacity);
  if (buffer == NULL) {
    fprintf(stderr, "failed to allocate buffer of size: %lu\n",
            (long unsigned)new_capacity);
    exit(1);
  }
}

char *xgetwd(void) {
  while (getcwd(buffer, capacity) == NULL) {
    if (errno != ERANGE) {
      perror("failed to getcwd");
      exit(1);
    }
    grow_buffer(capacity << 1);
  }
  return buffer;
}

int main(void) {
  int c;
  int gt_pathmax = 0;
  size_t initial_pathlen = 0;
  size_t pathlen = 0;

  grow_buffer(PATH_MAX);
  initial_pathlen = strlen(xgetwd());
  for (;;) {
    if (mkdir(DIR_NAME, 0755) == -1) {
      perror("failed to create new directory");
      return 1;
    }

    if (chdir(DIR_NAME) == -1) {
      perror("failed to chdir");
      return 1;
    }

    pathlen = strlen(xgetwd()) - initial_pathlen;
    if (pathlen >= PATH_MAX)
      break;

    if (pathlen + DIR_NAME_LEN >= PATH_MAX) {
      printf(
          "the creation of the next child directory will exceed the PATH_MAX\n"
          "continue? [Y/n]: ");
      fflush(stdout);

      c = getchar();
      if (c == EOF) {
        printf("failed to read from stdin: %s\n",
               ferror(stdin) ? strerror(errno) : "end of file reached");
        return 1;
      } else if (c == 'n' || c == 'N') {
        break;
      }
    }
  }
  printf("stopping...\n"
         "path length: %lu\n",
         (long unsigned)pathlen);
  sleep(1);
  return 0;
}
