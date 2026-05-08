#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void setup(void);
int child_main(void);

int main(void) {
  int rv;

  setup();

  switch (fork()) {
  case -1:
    perror("failed to fork");
    return 1;

  case 0:
    exit(child_main());
  }

  /* wait for child to chroot and chdir */
  puts("parent: sleeping...");
  sleep(1);
  puts("parent: woke up!");

  puts("parent: moving mydir out of new-root...");
  if (rename("new-root/mydir", "mydir")) {
    perror("failed to rename");
    return 1;
  }

  puts("parent: waiting for child to exit...");

  if (wait(&rv) == -1) {
    perror("parent: failed to wait for child process");
    return 1;
  }

  return rv;
}

void xmkdir(const char *path);

#define CREATE_DIR(fname, on_not_exists, on_file_exists)                       \
  do {                                                                         \
    if (stat((fname), &sb) == -1) {                                            \
      if (errno == ENOENT) {                                                   \
        do                                                                     \
          on_not_exists while (0);                                             \
      } else {                                                                 \
        fprintf(stderr, "failed to get %s status: %s\n", (fname),              \
                strerror(errno));                                              \
        exit(1);                                                               \
      }                                                                        \
    } else if (!S_ISDIR(sb.st_mode)) {                                         \
      fprintf(stderr, "%s already exists and is not a directory\n", (fname));  \
      exit(1);                                                                 \
    } else {                                                                   \
      do                                                                       \
        on_file_exists while (0);                                              \
    }                                                                          \
  } while (0)

void setup(void) {
  struct stat sb;

  puts("setup called");

  CREATE_DIR(
      "new-root",
      {
        /* if dir does not exists */
        xmkdir("new-root");
        xmkdir("new-root/mydir");
      },
      {
        /* if dir exists */
        CREATE_DIR("new-root/mydir",
                   {
                     /* if dir does not exists */
                     xmkdir("new-root/mydir");
                   },
                   {});
      });

  if (rmdir("mydir") == -1) {
    if (errno != ENOENT) {
      perror("failed to remove mydir");
      exit(1);
    }
  }
}

void xmkdir(const char *path) {
  if (mkdir(path, 0755) == -1) {
    fprintf(stderr, "failed to mkdir: %s: %s\n", path, strerror(errno));
    exit(1);
  }
}

#define open_file() open("../outsidechroot", O_RDONLY)

int child_main(void) {
  int rv;
  int fd;
  ssize_t n;
  char buffer[4096];

  rv = 0;

  puts("child: chroot...");
  if (chroot("new-root")) {
    perror("child: failed to chroot");
    return 1;
  }

  puts("child: chdir...");
  if (chdir("/mydir")) {
    perror("child: failed to chdir");
    return 1;
  }

  fd = open_file();
  if (fd != -1) {
    fprintf(stderr, "child: expected to not find the before the directory "
                    "being moved out of chroot\n");
    return 1;
  }

  puts("child: sleeping...");
  sleep(2);
  puts("child: woke up!");

  fd = open_file();
  if (fd == -1) {
    perror("child: failed to open file");
    return 1;
  }

  puts("child: reading file...");
  while ((n = read(fd, buffer, sizeof(buffer))) > 0)
    write(STDOUT_FILENO, buffer, n);

  if (n == -1) {
    perror("child: failed to read from file");
    rv = 1;
  }

  if (close(fd)) {
    perror("child: failed to close file");
    rv = 1;
  }

  return rv;
}
