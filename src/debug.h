#include <stdio.h>

#ifdef DEBUG
#if(DEBUG)
#define DBGPRNT(...) fprintf(stderr, "DBG:");fprintf(stderr, __VA_ARGS__)
#else
#define DBGPRNT(...) 0
#endif
#else
#define DBGPRNT(...) 0
#endif
