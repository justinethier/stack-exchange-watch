#ifndef __HTTP_H__
#define __HTTP_H__
 
struct MemoryStruct {
  char *memory;
  size_t size;
};
 
struct MemoryStruct *http_get(const char *url);

#endif
