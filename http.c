/**
 * This file is part of stack-watch, a utility to monitor Stack Exchange.
 * Copyright (C) 2014 Justin Ethier <justinethier@github>
 *
 * Please use github to submit patches and bug reports:
 * https://github.com/justinethier/stack-exchange-watch
 *
 * This code is based on memory-only curl example from 
 * http://curl.haxx.se/libcurl/c/example.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include <curl/curl.h>
#include "http.h"
 
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}
 
struct MemoryStruct *http_get(const char *url) { 
  CURL *curl_handle;
  CURLcode res;
 
  struct MemoryStruct *chunk = 
    (struct MemoryStruct *)malloc(sizeof(struct MemoryStruct));
 
  chunk->memory = malloc(1);  /* will be grown as needed by the realloc above */ 
  chunk->size = 0;    /* no data at this point */ 
 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* init the curl session */ 
  curl_handle = curl_easy_init();
 
  /* specify URL to get */ 
  curl_easy_setopt(curl_handle, CURLOPT_URL, url); //"http://www.example.com/");

  /* API data is compressed */
  curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "gzip");
 
  /* send all data to this function  */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
 
  /* we pass our 'chunk' struct to the callback function */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)chunk);
 
  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */ 
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
 
  /* get it! */ 
  res = curl_easy_perform(curl_handle);
 
  /* check for errors */ 
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
  }
  else {
    /*
     * Now, our chunk.memory points to a memory block that is chunk.size
     * bytes big and contains the remote file.
     *
     * Do something nice with it!
     */ 
 
    //printf("%lu bytes retrieved\n", (long)chunk->size);
  }
 
  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);
 
//  if(chunk.memory)
//    free(chunk.memory);
 
  /* we're done with libcurl, so clean it up */ 
  curl_global_cleanup();
 
  return chunk;
}

// int main(void)
// {
//   char url[] = "http://api.stackexchange.com/2.2/questions?page=1&pagesize=10&order=desc&min=10&sort=activity&tagged=a-song-of-ice-and-fire&site=scifi";
//   struct MemoryStruct *raw = http_get(url);
//   printf("%s\n", raw->memory);
// 
//   if (raw->memory) free(raw->memory);
//   free(raw);
// }
