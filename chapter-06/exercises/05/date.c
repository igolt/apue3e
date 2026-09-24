/* Write a program to obtain the current time and print it using strftime, so
 * that it looks like the default output from date(1). Set the TZ environment
 * variable to different values and see what happens. */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
  time_t t;
  const char *tz_env;
  struct tm *tm;
  char buffer[64];

  if (time(&t) == -1) {
    perror("failed to get time");
    return 1;
  }

  tm = localtime(&t);
  if (tm == NULL) {
    perror("failed to get broken down time");
    return 1;
  }

  assert(strftime(buffer, sizeof(buffer), "%a %b %d %T %z %Y", tm));

  tz_env = getenv("TZ");
  if (tz_env) {
    printf("%s: ", tz_env);
  }
  puts(buffer);

  return 0;
}
