#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define WRITE_STR_LITERAL(fd, s)                                               \
  (write((fd), (s), sizeof(s) - 1) != sizeof(s) - 1)

int main(void) {
  int fd1;

  fd1 = creat("/tmp/apue-3.6", 0644);
  if (fd1 == -1) {
    perror("failed to create file");
    return 1;
  }

  if (WRITE_STR_LITERAL(fd1, "APUE\n")) {
    perror("failed to initialize file data");
    return 1;
  }
  close(fd1);

  fd1 = open("/tmp/apue-3.6", O_RDWR | O_APPEND);
  if (fd1 == -1) {
    perror("failed to open file to append");
    return 1;
  }

  if (lseek(fd1, 0, SEEK_SET) == -1) {
    perror("failed to seek");
    return 1;
  }

  if (WRITE_STR_LITERAL(fd1, "EUPA\n")) {
    perror("failed to append data");
    return 1;
  }
  close(fd1);

  return 0;
}
