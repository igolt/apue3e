/* BSD-based systems provide a function called funopen that allows us to
intercept read, write, seek, and close calls on a stream. Use this function to
implement fmemopen for FreeBSD and Mac OS X. */

/* The implementation tries to mimic GNU C Library 2.39 behavior */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#ifndef __FreeBSD__
#include <bsd/stdio.h>
#else
#include <stdio.h>
#endif

#define MAX_ITER 20

#ifndef USE_STDIO_FMEMOPEN
#ifdef fmemopen
#undef fmemopen
#endif

#define fmemopen myfmemopen
#endif

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define strlitlen(lit) (sizeof(lit) - 1)

#define streq(s1, s2) (strcmp((s1), (s2)) == 0)
#define strneq(s1, s2, n) (strncmp((s1), (s2), (n)) == 0)

static FILE *myfmemopen(void *buf, size_t size, const char *mode);

static void test_fmemopen_given_valid_modes(void) {
  size_t i;
  FILE *fp;
  char buffer[1];
  const char *valid_modes[] = {"r", "rb", "r+", "r+b", "rb+",
                               "w", "w+", "wb", "w+b", "wb+",
                               "a", "ab", "a+", "a+b", "ab+"};

  errno = 0;
  for (i = 0; i < ARRAY_SIZE(valid_modes); i++) {
    fp = fmemopen(buffer, sizeof(buffer), valid_modes[i]);

    assert(fp);
    assert(fclose(fp) == 0);
    assert(errno == 0);
  }
}

static void test_fmemopen_given_invalid_mode(void) {
  FILE *fp;
  char buffer[1];

  errno = 0;
  fp = fmemopen(buffer, sizeof(buffer), "invalid");

  assert(fp == NULL);
  assert(errno == EINVAL);
}

static void test_fmemopen_given_null_and_positive_size(void) {
  FILE *fp;

  errno = 0;
  fp = fmemopen(NULL, 1, "r+");

  assert(fp);
  assert(errno == 0);
  assert(fclose(fp) == 0);
}

static void test_seek_flushs_written_data(void) {
  FILE *fp;
  const char data[] = "Lorem ipsum";
  char buffer[sizeof(data)];

  fp = fmemopen(buffer, sizeof(buffer), "w");

  assert(fp);
  assert(fputs(data, fp) >= 0);
  assert(fseek(fp, -(int)strlen(data), SEEK_CUR) == 0);
  assert(streq(buffer, data));
  assert(fclose(fp) == 0);
}

static void test_fgets_all_lines_from_memory_stream(void) {
#define FIRST_LINE "Lorem ipsum\n"
#define SECOND_LINE "Dolor sit amet\n"

  FILE *fp;
  char *result;
  char line[16];
  char buffer[] = FIRST_LINE SECOND_LINE;

  fp = fmemopen(buffer, sizeof(buffer) - 1, "r");

  assert(fp);

  assert(fgets(line, sizeof(line), fp));
  assert(streq(line, FIRST_LINE));

  assert(fgets(line, sizeof(line), fp));
  assert(streq(line, SECOND_LINE));

  assert(fgets(line, sizeof(line), fp) == NULL);

  assert(fclose(fp) == 0);

#undef FIRST_LINE
#undef SECOND_LINE
}

static void test_write_to_memory_stream(void) {
  FILE *fp;
  const char data[] = "Lorem ipsum";
  char buffer[sizeof(data)];

  fp = fmemopen(buffer, sizeof(buffer), "w");

  assert(fp);
  assert(fputs(data, fp) >= 0);
  assert(fclose(fp) == 0);
  assert(streq(data, buffer));
}

static void test_write_given_data_length_is_equal_to_buffer_capacity(void) {
  FILE *fp;
  const char data[] = "Lorem ipsum";
  char buffer[strlitlen(data)];

  fp = fmemopen(buffer, sizeof(buffer), "w+");

  assert(fp);
  assert(fputs(data, fp) >= 0);
  assert(fclose(fp) == 0);
  assert(buffer[sizeof(buffer) - 1] == '\0');
  assert(strneq(buffer, data, strlen(buffer)));
}

static void test_append_past_buffer_end(void) {
  FILE *fp;
  const char data[] = "Lorem ipsum";
  char memory[strlitlen(data)];

  memset(memory, 'x', sizeof(memory));
  fp = fmemopen(memory, sizeof(memory), "a");

  assert(fp);
  assert(fputs(data, fp) >= 0);
  assert(fclose(fp) != 0);
  assert(errno == ENOSPC);
}

static void test_read_from_stream_not_openned_for_read(void) {
  size_t i, n;
  FILE *fp;
  const char *write_modes[] = {"w", "wb", "a", "ab"};
  char buffer[8];
  char memory[16];

  memset(memory, 'u', sizeof(memory));

  for (i = 0; i < ARRAY_SIZE(write_modes); i++) {
    fp = fmemopen(memory, sizeof(memory), write_modes[i]);
    errno = 0;

    assert(fp);
    assert(fread(buffer, 1, sizeof(buffer), fp) == 0);
    assert(ferror(fp));
    assert(errno == EBADF);
  }
}

static void test_write_to_stream_not_openned_for_writting(void) {
  size_t i;
  FILE *fp;
  const char *write_modes[] = {"r", "rb"};
  char buffer[8];
  char memory[16];

  char fbuffer[BUFSIZ];

  memset(memory, 'u', sizeof(memory));
  memset(buffer, 'a', sizeof(buffer));

  for (i = 0; i < ARRAY_SIZE(write_modes); i++) {
    fp = fmemopen(memory, sizeof(memory), write_modes[i]);

    assert(fp);

    fwrite(buffer, 1, sizeof(buffer), fp);
    assert(fflush(fp));
    assert(ferror(fp));
    assert(errno == EBADF);
  }

  /* Verify memory content */
  for (i = 0; i < sizeof(memory); i++) {
    assert(memory[i] == 'u');
  }
}

#define RUN_TEST(test)                                                         \
  do {                                                                         \
    printf("Running test: " #test "\n");                                       \
    fflush(stdout);                                                            \
    (test)();                                                                  \
    printf("          OK: " #test "\n\n");                                     \
    fflush(stdout);                                                            \
  } while (0);

int main(void) {
  RUN_TEST(test_fmemopen_given_valid_modes);
  RUN_TEST(test_fmemopen_given_invalid_mode);
  RUN_TEST(test_fmemopen_given_null_and_positive_size);
  RUN_TEST(test_seek_flushs_written_data);
  RUN_TEST(test_fgets_all_lines_from_memory_stream);
  RUN_TEST(test_write_to_memory_stream);
  RUN_TEST(test_write_given_data_length_is_equal_to_buffer_capacity);
  RUN_TEST(test_append_past_buffer_end);
  RUN_TEST(test_read_from_stream_not_openned_for_read);
  RUN_TEST(test_write_to_stream_not_openned_for_writting);
  return 0;
}

typedef struct fmem {
  char *buf;
  char *pos;
  char *end;
  char *data_end;
  int flags;
} fmem_t;

static int fmem_read(void *cookie, char *buf, int n);
static int fmem_write(void *cookie, const char *buf, int n);
static off_t fmem_seek(void *cookie, off_t offset, int whence);
static int fmem_close(void *cookie);

#define _R 0x01 /* read */
#define _W 0x02 /* write */
#define _A 0x04 /* append */
#define _F 0x08 /* should free the buffer */
#define _N 0x10 /* should append null byte on write and update data_end */

static FILE *myfmemopen(void *buf, size_t size, const char *mode) {
  int truncate;
  int flags;
  fmem_t *fmem;

  truncate = 0;
  if (streq(mode, "r") || streq(mode, "rb")) {
    flags = _R;
  } else if (streq(mode, "r+") || streq(mode, "r+b") || streq(mode, "rb+")) {
    flags = _R | _W;
  } else if (streq(mode, "w") || streq(mode, "wb")) {
    flags = _W | _N;
  } else if (streq(mode, "w+") || streq(mode, "w+b") || streq(mode, "wb+")) {
    flags = _R | _W | _N;
    truncate = 1;
  } else if (streq(mode, "a") || streq(mode, "ab")) {
    flags = _A | _N;
  } else if (streq(mode, "a+") || streq(mode, "a+b") || streq(mode, "ab+")) {
    flags = _R | _A | _N;
  } else {
    errno = EINVAL;
    return NULL;
  }

  /* alloc structure */
  fmem = malloc(sizeof(*fmem));
  if (fmem == NULL)
    return NULL;

  /* use specified `buf` or alloc a buffer of size `size` if `buf` is NULL */
  if (buf) {
    fmem->buf = buf;
  } else {
    flags |= _F; /* add free flag */
    fmem->buf = calloc(size, 1);
    if (fmem->buf == NULL) {
      free(fmem);
      return NULL;
    }
  }

  fmem->end = buf + size;

  if (flags & _A) {
    fmem->pos = memchr(fmem->buf, '\0', size);
    if (fmem->pos == NULL)
      fmem->pos = fmem->end;
    fmem->data_end = fmem->pos;
  } else {
    fmem->pos = fmem->buf;
    fmem->data_end = flags & _N ? fmem->pos : fmem->end;
  }
  fmem->flags = flags;

  if (truncate) {
    *fmem->pos = '\0';
  }
  return funopen(fmem, fmem_read, fmem_write, fmem_seek, fmem_close);
}

static int fmem_read(void *cookie, char *buf, int n) {
  int diff;
  fmem_t *fmem = (fmem_t *)cookie;

  if ((fmem->flags & _R) == 0) {
    errno = EBADF;
    return -1;
  }

  diff = fmem->data_end - fmem->pos;
  n = MIN(diff, n);
  if (n > 0) {
    memcpy(buf, fmem->pos, n);
    fmem->pos += n;
  }
  return n;
}

static int fmem_write(void *cookie, const char *buf, int n) {
  int diff;
  fmem_t *fmem = (fmem_t *)cookie;

  assert(n > 0);

  if (fmem->flags & _A) {
    puts("aqui");
    fmem->pos = fmem->data_end;
  } else if ((fmem->flags & _W) == 0) {
    puts("não está aberto para escrita");
    errno = EBADF;
    return 0;
  }

  puts("passou para escrever");

  diff = fmem->end - fmem->pos;
  if (fmem->flags & _N) {
    if (n < diff) {
      memcpy(fmem->pos, buf, n);
      fmem->pos += n;
      *fmem->pos = '\0';
    } else if (diff) {
      n = diff - 1;
      if (n) {
        memcpy(fmem->pos, buf, n);
      }
      fmem->pos[n] = '\0';
      fmem->pos += diff;
      n++;
    }
    if (fmem->pos > fmem->data_end) {
      fmem->data_end = fmem->pos;
    }
  } else {
    n = MIN(diff, n);
    if (n) {
      memcpy(fmem->pos, buf, n);
      fmem->pos += n;
    }
  }
  return n;
}

static off_t fmem_seek(void *cookie, off_t offset, int whence) {
  char *pos;
  fmem_t *fmem = (fmem_t *)cookie;

  switch (whence) {
  case SEEK_CUR:
    pos = fmem->pos;
    break;
  case SEEK_END:
    pos = fmem->end;
    break;
  case SEEK_SET:
    pos = fmem->buf;
    break;
  default:
    errno = EINVAL;
    return -1;
  }

  pos += offset;
  if (pos < fmem->buf || pos > fmem->end) {
    errno = EINVAL;
    return -1;
  }
  fmem->pos = pos;
  return fmem->pos - fmem->buf;
}

static int fmem_close(void *cookie) {
  fmem_t *fmem = (fmem_t *)cookie;

  if (fmem->flags & _F)
    free(fmem->buf);
  free(fmem);
  return 0;
}
