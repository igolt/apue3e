/* Implement setbuf using setvbuf */

#include <stdio.h>

void mysetbuf(FILE *stream, char *buf) {
  setvbuf(stream, buf, buf ? _IOFBF : _IONBF, BUFSIZ);
}
