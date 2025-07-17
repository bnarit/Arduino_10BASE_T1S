
#include "t1s_adaptor.h"
char *t1s_lwip_itoa(char *buf, size_t bufsize, int num) {
  snprintf(buf, bufsize, "%d", num);  // base 10 only
  return buf;
}