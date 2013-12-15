#ifndef ERROR_H
#define ERROR_H

static inline void error_out(const char *message)
{
    fprintf(stderr, "ERROR: %s\n", message);
}

#endif

