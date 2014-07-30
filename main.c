#include <curl/curl.h>
#include <json/json.h>
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "http.h"
#include "se-api.h"
#include "util.h"

static char *version = "0.1.0";

char *apiURL(int page_size, const char *site, const char *tags) {
  char urlf[] = "http://api.stackexchange.com/2.2/questions?page=1&pagesize=%d&order=desc&sort=activity&tagged=%s&site=%s";
  char *url = NULL;
  int maxlen = strlen(site) + strlen(tags) + strlen(urlf) + 10;

  url = (char *)calloc(maxlen, sizeof(char));
  snprintf(url, maxlen, urlf, page_size, tags, site);

  return url;
}

void watch(int page_size, int poll_rate_mins, 
           const char *site, const char *tags) {
  char *url = apiURL(page_size, site, tags);
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

  free(url);
  se_free_questions(oldQs, numOldQs);
  se_free_questions(newQs, numNewQs);
}

void usage() {
  printf("Usage: stack-watch [OPTION]...                                                  \n");
  printf("Monitor question activity on a Stack Exchange site.                             \n");
  printf("                                                                                \n");
  printf("  -s, --site        Site to monitor                                             \n");
  printf("  -t, --tagged      Tag(s) to monitor                                           \n");
  printf("  -p, --pagesize    Maximum number of questions to monitor at a time            \n");
  printf("  -r, --rate        Rate at which to check for new activity, in minutes.        \n");
  printf("                    This cannot be set less than 1 minute.                      \n");
  printf("  --help            Display usage information                                   \n");
  printf("                                                                                \n");
  printf("With no OPTION, TBD                                                             \n");
  printf("                                                                                \n");
  printf("Examples:                                                                       \n");
  printf("  stack-watch TBD                                                               \n");
  printf("                                                                                \n");
  printf("Report bugs to: <https://github.com/justinethier/stack-exchange-watch/issues>   \n");
  printf("Project home page: <https://github.com/justinethier/stack-exchange-watch>       \n");
  printf("Copyright (C) Justin Ethier, 2014                                               \n");
}

int main(int argc, char **argv) {
  int c;
  int pollrate = 15; // mins
  int pagesize = 10;
  char *tags = "";
  char *site = "stackoverflow";

  static struct option long_options[] =
  {
    {"help",        no_argument, NULL, 'h'},
    {"pagesize",    required_argument, NULL, 'p'},
    {"rate",        required_argument, NULL, 'r'},
    {"site",        required_argument, NULL, 's'},
    {"tagged",      required_argument, NULL, 't'},
    {"version",     no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0}
  };

  while ((c = getopt_long(argc, argv, "p:r:s:t:v", long_options, NULL)) != -1) {
    switch (c) {
//      case 'a':
//        aflag = 1;
//        break;
      case 'v':
printf("stack-watch version %s\n", version);
// TODO: display author, home page, etc
        return 0;
      case 'h':
        usage();
        return 0;
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

  watch(pagesize, pollrate, site, tags);
  return 0;
}

