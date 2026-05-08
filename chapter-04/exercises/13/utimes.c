#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>

#define FNAME "utimes.c"

#define nsec_to_usec(ns) ((ns) / 1000)

void print_times(struct stat *sb);
void statfile(struct stat *sb);

int main(void) {
  struct timespec times[2];
  struct timeval tvals[2];
  struct stat sb;

  statfile(&sb);
  puts("=== Before utimensat ===");
  print_times(&sb);

  times[0].tv_nsec = UTIME_OMIT;
  times[1].tv_nsec = UTIME_OMIT;
  if (utimensat(AT_FDCWD, FNAME, times, 0) == -1) {
    perror("utimensat");
    return 1;
  }

  statfile(&sb);
  puts("=== After utimensat ===");
  print_times(&sb);

  tvals[0].tv_sec = sb.st_atime;
  tvals[0].tv_usec = nsec_to_usec(sb.st_atim.tv_nsec);
  tvals[1].tv_sec = sb.st_mtime;
  tvals[1].tv_usec = nsec_to_usec(sb.st_mtim.tv_nsec);
  if (utimes(FNAME, tvals) == -1) {
    perror("utimes");
    return 1;
  }

  statfile(&sb);
  puts("=== After utimes ===");
  print_times(&sb);
  return 0;
}

void statfile(struct stat *sb) {
  if (lstat(FNAME, sb) == -1) {
    perror("failed to lstat");
    exit(1);
  }
}

void print_timespec(const char *timelabel, struct timespec ts);

void print_times(struct stat *sb) {
  print_timespec("Modification time", sb->st_mtim);
  print_timespec("Access time", sb->st_atim);
  print_timespec("Change time", sb->st_ctim);
}

void print_timespec(const char *timelabel, struct timespec ts) {
  size_t res;
  struct tm tm;
  char buffer[sizeof("YYYY-mm-dd HH:MM:SS.NNNNNNNNN shhmm")];

  if (localtime_r(&ts.tv_sec, &tm) == NULL) {
    perror("localtime_r");
    exit(1);
  }

  res = strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
  res += sprintf(buffer + res, ".%09lu ", ts.tv_nsec);
  strftime(buffer + res, sizeof(buffer) - res, "%z", &tm);

  printf("%17s: %s\n", timelabel, buffer);
}
