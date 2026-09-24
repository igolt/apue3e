/* If you have superuser access and your system uses shadow passwords, implement
 * the previous exercise. */

#include <shadow.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, const char *argv[]) {
  struct spwd *spwd;

  if (argc != 2) {
    fprintf(stderr, "usage: %s USERNAME\n", argv[0]);
    return 1;
  }

  spwd = getspnam(argv[1]);
  if (spwd == NULL) {
    perror("failed to get encrypted password");
    return 1;
  }
  puts(spwd->sp_pwdp);
  return 0;
}
