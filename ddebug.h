#ifndef _DDEBUG_H_
#define _DDEBUG_H_

#include <stdio.h>

#if DDEBUG

#define DD(...) fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, " at %s line %d\n", __FILE__, __LINE__);

#else /* DDEBUG */

#define DD

#endif /* DDEBUG */

#endif /* _DDEBUG_H_ */

