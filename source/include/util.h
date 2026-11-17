#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

void die(int ret, const char *fmt, ...);
void _log(FILE *fd, const char *fmt, ...);
void sig_handler(int s);

#define LENGTH(x) (sizeof(x) / sizeof((x)[0]))

#endif
