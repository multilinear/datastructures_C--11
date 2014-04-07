#include <stdlib.h>
#include <stdio.h>

#define PANIC(msg) fprintf(stderr, "PANIC: %s() [%s:%d] %s\n", __FUNCTION__, __FILE__, __LINE__, msg); exit(1);
