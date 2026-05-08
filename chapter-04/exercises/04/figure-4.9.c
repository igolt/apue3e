#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

#define RWRWRW (S_IRWXU | S_IRWXG | S_IRWXO)

int main(void) {
  umask(0);
  if (creat("foo", RWRWRW) == -1)
    perror("creat error for foo");

  umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (creat("bar", RWRWRW) == -1)
    perror("creat error for bar");
  return 0;
}
