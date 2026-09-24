/* Some UNIX system implementations purposely arrange that, when a program is
 * executed, location 0 in the data segment is not accessible. Why?
 */
#include <stdio.h>

typedef void (*Exitfunc)(void);

extern int atexit(Exitfunc);
extern void exit(int);

static void exit_handler1(void) { puts("Exit handler 1"); }

static void exit_handler2(void) { puts("Exit handler 2"); }

#define AT_EXIT_OR_ERR(fn)                                                     \
  do {                                                                         \
    if (atexit(fn)) {                                                          \
      perror("failed to register " #fn);                                       \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

int main(void) {
  AT_EXIT_OR_ERR(exit_handler2);
  AT_EXIT_OR_ERR(exit_handler1);
  AT_EXIT_OR_ERR(exit_handler1);
  return 0;
}
