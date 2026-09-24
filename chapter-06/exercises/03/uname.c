/* Write a program that calls uname and prints all the fields in the utsname
 *structure. Compare the output to the output from the uname(1) command.

 * Comparing the output:

$ gcc uname.c
$ ./a.out
sysname: Linux
nodename: ssys
release: 7.0.0-31-generic
version: #31~24.04.1-Ubuntu SMP PREEMPT_DYNAMIC Mon Aug 10 09:38:02 UTC 2
machine: x86_64
$ uname
Linux

*/

#include <stdio.h>
#include <sys/utsname.h>

int main(void) {
#define PFIELD(field) printf(#field ": %s\n", buf.field)

  struct utsname buf;

  if (uname(&buf) == -1) {
    perror("failed to get kernel information");
    return 1;
  }

  PFIELD(sysname);
  PFIELD(nodename);
  PFIELD(release);
  PFIELD(version);
  PFIELD(machine);

  return 0;
}
