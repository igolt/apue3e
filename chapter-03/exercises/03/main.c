#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH "/tmp/apue-3.3"
#define OFLAGS O_RDONLY

void test(const char *path, int oflags) {
  int fd1, fd2, fd3;
  int fd1_fflags, fd2_fflags, fd3_fflags;
  int fd1_flflags, fd2_flflags, fd3_flflags;

  fd1 = open(path, oflags);
  fd2 = dup(fd1);
  fd3 = open(path, oflags);

#define PFFLAGS()                                                              \
  do {                                                                         \
    fd1_fflags = fcntl(fd1, F_GETFD);                                          \
    fd2_fflags = fcntl(fd2, F_GETFD);                                          \
    fd3_fflags = fcntl(fd3, F_GETFD);                                          \
    printf("fd1 fd flags: %d\n", fd1_fflags);                                  \
    printf("fd2 fd flags: %d\n", fd2_fflags);                                  \
    printf("fd3 fd flags: %d\n", fd3_fflags);                                  \
    printf("==============================\n");                                \
  } while (0)

  PFFLAGS();
  fcntl(fd1, F_SETFD, FD_CLOEXEC);
  PFFLAGS();

#define PFLFLAGS()                                                             \
  do {                                                                         \
    fd1_flflags = fcntl(fd1, F_GETFL);                                         \
    fd2_flflags = fcntl(fd2, F_GETFL);                                         \
    fd3_flflags = fcntl(fd3, F_GETFL);                                         \
    printf("fd1 file flags: %d\n", fd1_flflags);                               \
    printf("fd2 file flags: %d\n", fd2_flflags);                               \
    printf("fd3 file flags: %d\n", fd3_flflags);                               \
    printf("==============================\n");                                \
  } while (0)

  PFLFLAGS();
  fcntl(fd1, F_SETFL, O_NONBLOCK);
  PFLFLAGS();

  close(fd1);
  close(fd2);
  close(fd3);
}

void setup(void) {
  int fd;

  if ((fd = creat(PATH, 0644)) == -1) {
    perror("failed to setup");
    exit(1);
  }
  close(fd);
}

int main(void) {
  setup();
  test(PATH, OFLAGS);
  return 0;
}
