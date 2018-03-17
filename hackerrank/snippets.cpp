// Initialise srand() using current time.

#include <sys/time.h>

struct timeval time;
gettimeofday(&time,NULL);
srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

