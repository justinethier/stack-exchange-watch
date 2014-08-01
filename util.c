/**
 * This file is part of stack-watch, a utility to monitor Stack Exchange.
 * Copyright (C) 2014 Justin Ethier <justinethier@github>
 *
 * Please use github to submit patches and bug reports:
 * https://github.com/justinethier/stack-exchange-watch
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Read contents of file into a new byte array
 *
 * Returns
 *  - Pointer to newly-allocated buffer
 *  - Size of buffer as an out parameter
 */
char *get_file_contents(const char *filename, long *outpSize) {
  char *source = NULL;
  FILE *fp = fopen(filename, "r");
  if (fp != NULL) {
      /* Go to the end of the file. */
      if (fseek(fp, 0L, SEEK_END) == 0) {
          /* Get the size of the file. */
          long bufsize = ftell(fp);
          if (bufsize == -1) { /* Error */ }
  
          /* Allocate our buffer to that size. */
          source = calloc(sizeof(char), (bufsize + 1));
  
          /* Go back to the start of the file. */
          if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }
  
          /* Read the entire file into memory. */
          size_t newLen = fread(source, sizeof(char), bufsize, fp);
          if (newLen == 0) {
              fputs("Error reading file", stderr);
          } else {
// Not so safe, can cause segfault in Linux
//              source[++newLen] = '\0'; /* Just to be safe. */
          }

          // Let caller know the size of the buffer
          if (outpSize != NULL) {
            *outpSize = bufsize;
          }
      }
      fclose(fp);
  }
  
  //free(source); /* Don't forget to call free() later! */
  return(source);
}

/**
 * Allocate a new filename with the given extension,
 * or NULL if an error occurs
 */
char *fname_with_ext(const char *fname, const char *ext){
  int len = strlen(fname);

  if (len > 4 && fname[len - 4] == '.'){
    char *buf = malloc(len+1); 
    strncpy(buf, fname, len+1);
    strncpy(buf + (len - 3), ext, 3);
    return buf;
  }

  return NULL;
}

char *_strdup (const char *s) {
    char *d = malloc (strlen (s) + 1);   // Space for length plus nul
    if (d == NULL) return NULL;          // No memory
    strcpy (d,s);                        // Copy the characters
    return d;                            // Return the new string
}

