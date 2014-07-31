/**
 * This file is part of stack-watch, a utility to monitor Stack Exchange.
 * Copyright (C) 2014 Justin Ethier <justinethier@github>
 *
 * Please use github to submit patches and bug reports:
 * https://github.com/justinethier/stack-exchange-watch
 */
#ifndef __UTIL__
#define __UTIL__

#define DEBUG 0
#define tracef(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)
#define trace(fmt) \
            do { if (DEBUG) fprintf(stderr, fmt); } while (0)

char *getFileContents(const char *filename, long *outpSize);
char *fnameWithExt(const char *fname, const char *ext);
char *_strdup (const char *s);
#endif
