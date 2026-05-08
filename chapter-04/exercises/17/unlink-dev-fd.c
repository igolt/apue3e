#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define DEV_FD_1 "/dev/fd/1"

void print_fd_stat(int fd);
void print_dev_fd_stat(int fd);

int main(void) {
  int fd;

  if (unlink(DEV_FD_1) == -1) {
    perror("failed to unlink");
  }

  fd = creat(DEV_FD_1, 0644);
  if (fd == -1) {
    perror("failed to create " DEV_FD_1);
    return 1;
  }

  printf("fd value: %d\n", fd);
  printf("fd refers to a tty: %s\n", isatty(fd) ? "yes" : "no");

  print_fd_stat(STDOUT_FILENO);
  print_fd_stat(fd);

  print_dev_fd_stat(STDOUT_FILENO);
  print_dev_fd_stat(fd);

  if (close(fd)) {
    perror("failed to close fd");
    return 1;
  }
  return 0;
}

#define PRINT_STAT(sb)                                                         \
  do {                                                                         \
    printf("Inode number: %lu\n", (unsigned long)sb.st_ino);                   \
    printf("Size: %lu\n", (unsigned long)sb.st_size);                          \
    printf("User ID: %lu\n", (unsigned long)sb.st_uid);                        \
    printf("Group ID: %lu\n", (unsigned long)sb.st_gid);                       \
  } while (0)

void print_fd_stat(int fd) {
  struct stat sb;

  if (fstat(fd, &sb) == -1) {
    fprintf(stderr, "failed to get file information for fd: %d: %s\n", fd,
            strerror(errno));
    return;
  }

  puts("========================");
  printf("fstat for fd: %d\n", fd);
  PRINT_STAT(sb);
  puts("========================");
}

void print_link_info(const char *slink);

void print_dev_fd_stat(int fd) {
  char fname[PATH_MAX];
  struct stat sb;

  sprintf(fname, "/dev/fd/%d", fd);

  if (lstat(fname, &sb) == -1) {
    fprintf(stderr, "failed to get file information for file: %s: %s\n", fname,
            strerror(errno));
    return;
  }

  puts("========================");
  printf("lstat for %s\n", fname);
  PRINT_STAT(sb);
  if (S_ISLNK(sb.st_mode)) {
    print_link_info(fname);
  }
  puts("========================");
}

void print_link_info(const char *slink) {
  char points_to[PATH_MAX] = {0};

  if (readlink(slink, points_to, PATH_MAX) == -1) {
    perror("failed to readlink");
    return;
  }
  printf("Points to: %s\n", points_to);
}
