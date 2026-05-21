#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 4

#define err_sys(msg)                                                           \
  do {                                                                         \
    perror((msg));                                                             \
    exit(1);                                                                   \
  } while (0)

int main(void) {
  char buf[MAXLINE];

  while (fgets(buf, MAXLINE, stdin) != NULL)
    if (fputs(buf, stdout) == EOF)
      err_sys("output error");

  if (ferror(stdin))
    err_sys("input error");
  return 0;
}
