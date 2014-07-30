#include <curl/curl.h>
#include <json/json.h>
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "http.h"
#include "se-api.h"
#include "util.h"

// TODO: include command line arguments.
// could specify API string, poll rate (in minutes, no less than 1) from cli
// if none are specified, maybe read from a .rc file?
// otherwise either give up or use default (not sure default search makes any sense, though. probably better to print link to the SE API query page)
void watch(int page_size, int poll_rate_mins) {
  char url[] = "http://api.stackexchange.com/2.2/questions?page=1&pagesize=10&order=desc&min=10&sort=activity&tagged=a-song-of-ice-and-fire&site=scifi";
  int numOldQs = 0;
  int numNewQs = 0;
  struct SeQuestion **oldQs = NULL;
  struct SeQuestion **newQs = NULL;
  struct MemoryStruct *apiData = NULL;

  while(1) {
    tracef("Woke up, checking %s\n", url);
    apiData = http_get(url);
    if (apiData == NULL || apiData->size == 0) {
        fprintf(stderr, "Unable to retrieve data from URL %s\n", url);
    } else {
        newQs = se_load(apiData->memory, &numNewQs, page_size);
        tracef("Received %d questions\n", numNewQs);
        if (oldQs != NULL) {
          trace("Checking for updates\n");
          se_check_for_updates(oldQs, numOldQs, newQs, numNewQs);
        }
        se_free_questions(oldQs, numOldQs);
        oldQs = newQs;
        numOldQs = numNewQs;
    }

    sleep(60 * poll_rate_mins);
  }

  se_free_questions(oldQs, numOldQs);
  se_free_questions(newQs, numNewQs);
}

int main(int argc, char **argv) {
  int c;
  int pollrate = 15; // mins
  int pagesize = 10;
  char *tags = "";
  char *site = "";

  while ((c = getopt (argc, argv, "p:r:s:t:")) != -1) {
    switch (c) {
//      case 'a':
//        aflag = 1;
//        break;
      case 'p':
        pagesize = atoi(optarg);
        break;
      case 'r':
        pollrate = atoi(optarg);
        if (pollrate < 1) {
            pollrate = 1;
        }
        break;
      case 's':
        site = optarg;
        break;
      case 't':
        tags = optarg;
        break;
      case '?':
        //if (optopt == 'p' || optopt == 'r' || optopt == 's' || optopt == 't')
        //  fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        //else if (isprint (optopt))
        //  fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        //else
        //  fprintf (stderr,
        //           "Unknown option character `\\x%x'.\n",
        //           optopt);
        return 1;
      default:
        abort ();
    }
  }

  printf("Rate = %d, Page Size = %d, Site = %s, Tags = %s\n", 
    pollrate, pagesize, site, tags);

// TODO: pass args to core function:
  watch(pagesize, pollrate);
  return 0;
}

