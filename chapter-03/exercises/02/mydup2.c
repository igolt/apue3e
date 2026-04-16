/* Write your own dup2 function that behaves the same way as the dup2 function
described in Section 3.12, without calling the fcntl function. Be sure to handle
errors correctly. */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

int mydup2_helper(int oldfd, int newfd) {
  int old_errno;
  int fd;

  fd = dup(oldfd);
  if (fd != -1 && fd != newfd) {
    newfd = mydup2_helper(oldfd, newfd);
    old_errno = errno;
    if (close(fd) == -1) {
      errno = old_errno;
    }
    return newfd;
  }
  return fd;
}

int mydup2(int oldfd, int newfd) {
  int old_errno;
  int fd;
  struct stat sb;
  struct rlimit rlim;

  old_errno = errno;
  if (getrlimit(RLIMIT_NOFILE, &rlim) == -1) {
    errno = old_errno;
  } else if (newfd >= rlim.rlim_cur) {
    errno = EBADF;
    return -1;
  }

  /* not a valid fd or kernel memory is out of memory (unlikely) */
  if (fstat(oldfd, &sb) == -1) {
    errno = EBADF;
    return -1;
  }

  if (oldfd == newfd) {
    return newfd;
  }

  old_errno = errno;
  if (close(newfd) == -1) {
    errno = old_errno;
  }
  return mydup2_helper(oldfd, newfd);
}

#define OLDFD 1
#define NEWFD 60

int main(void) {
  int fd;

  fd = mydup2(1, 1);
  assert(fd == 1);

  if (write(fd, "hello\n", sizeof("hello\n") - 1) != sizeof("hello\n") - 1) {
    perror("failed to write");
  }

  if (close(fd) == -1) {
    perror("failed to close");
  }
  return 0;
}
