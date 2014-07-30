
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

  struct SeQuestion **oldQs = 
    se_load(getFileContents("data/results.json", NULL), &numOldQs, 10);
  struct SeQuestion **newQs = 
    se_load(getFileContents("data/results2.json", NULL), &numNewQs, 10);
  se_check_for_updates(oldQs, numOldQs, newQs, numNewQs);

  se_free_questions(oldQs, numOldQs);
  se_free_questions(newQs, numNewQs);
}

int main() {
  test();
  return 0;
}

