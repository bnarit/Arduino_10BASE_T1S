#ifndef T1S_LWIP_ADAPTER_H
#define T1S_LWIP_ADAPTER_H
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
char *t1s_lwip_itoa(char *buf, size_t bufsize, int num);


#ifdef __cplusplus
}
#endif


#ifndef lwip_htonl
#define lwip_htonl(x) PP_HTONL(x)
#endif

#ifndef lwip_htons
#define lwip_htons(x) PP_HTONS(x)
#endif
#endif