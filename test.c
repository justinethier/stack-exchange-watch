/**
 * This file is part of stack-watch, a utility to monitor Stack Exchange.
 * Copyright (C) 2014 Justin Ethier <justinethier@github>
 *
 * Please use github to submit patches and bug reports:
 * https://github.com/justinethier/stack-exchange-watch
 */

#include <json/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "se-api.h"
#include "util.h"

// TODO: include command line arguments.
// could specify API string, poll rate (in minutes, no less than 1) from cli
// if none are specified, maybe read from a .rc file?
// otherwise either give up or use default (not sure default search makes any sense, though. probably better to print link to the SE API query page)
void test() {
  int numOldQs = 0;
  int numNewQs = 0;
  char *oldF = getFileContents("data/results.json", NULL);
  char *newF = getFileContents("data/results2.json", NULL);

  struct SeQuestion **oldQs = se_load(oldF, &numOldQs, 10);
  struct SeQuestion **newQs = se_load(newF, &numNewQs, 10);
  se_check_for_updates(oldQs, numOldQs, newQs, numNewQs);

  free(oldF);
  free(newF);
  se_free_questions(oldQs, numOldQs);
  se_free_questions(newQs, numNewQs);
}

int main() {
  test();
  return 0;
}

