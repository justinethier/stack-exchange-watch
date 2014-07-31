/**
 * This file is part of stack-watch, a utility to monitor Stack Exchange.
 * Copyright (C) 2014 Justin Ethier <justinethier@github>
 *
 * Please use github to submit patches and bug reports:
 * https://github.com/justinethier/stack-exchange-watch
 */

#ifndef __HTTP_H__
#define __HTTP_H__
 
struct MemoryStruct {
  char *memory;
  size_t size;
};
 
struct MemoryStruct *http_get(const char *url);

#endif
