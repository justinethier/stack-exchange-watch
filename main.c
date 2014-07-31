/**
 * This file is part of stack-watch, a utility to monitor Stack Exchange.
 * Copyright (C) 2014 Justin Ethier <justinethier@github>
 *
 * Please use github to submit patches and bug reports:
 * https://github.com/justinethier/stack-exchange-watch
 */
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

static char *versionStr = "0.1.0";

void version() {
  printf("stack-watch %s\n", versionStr);
  printf("Copyright (C) Justin Ethier, 2014                                               \n");
  printf("                                                                                \n");
  printf("Report bugs to: <https://github.com/justinethier/stack-exchange-watch/issues>   \n");
  printf("Project home page: <https://github.com/justinethier/stack-exchange-watch>       \n");
}

void usage() {
  printf("Usage: stack-watch [OPTION]...                                                  \n");
  printf("Automatically monitor Q&A activity on a Stack Exchange site.                    \n");
  printf("                                                                                \n");
  printf("  -a, --all         Display all matching questions, not just updates            \n");
  printf("  -p, --pagesize    Maximum number of questions to monitor at a time            \n");
  printf("  -r, --rate        Rate at which to check for new activity, in minutes.        \n");
  printf("                    This cannot be set less than 1 minute.                      \n");
  printf("  -s, --site        Site to monitor                                             \n");
  printf("  -t, --tagged      Tag(s) to monitor                                           \n");
  printf("  -v, --version     Display version information                                 \n");
  printf("      --help        Display usage information                                   \n");
  printf("                                                                                \n");
//  printf("With no OPTION, TBD                                                             \n");
//  printf("                                                                                \n");
  printf("Examples:                                                                       \n");
  printf("  stack-watch -s scifi -t asoiaf      Display updates under the \"asoiaf tag\"  \n");
  printf("                                      on the scifi site.                        \n");
  printf("  stack-watch -a -t http              Display most recent \"http\" questions on \n");
  printf("                                      the default site.                         \n");
  printf("                                                                                \n");
  printf("Report bugs to: <https://github.com/justinethier/stack-exchange-watch/issues>   \n");
  printf("Project home page: <https://github.com/justinethier/stack-exchange-watch>       \n");
  printf("Copyright (C) Justin Ethier, 2014                                               \n");
}

void watch(int page_size, int poll_rate_mins, 
           const char *site, const char *tags,
           int display_all) {
  char *url = se_api_url(page_size, site, tags);
  int numOldQs = 0;
  int numNewQs = 0;
  struct SeQuestion **oldQs = NULL;
  struct SeQuestion **newQs = NULL;
  struct MemoryStruct *apiData = NULL;

  while(1) {
    tracef("Woke up, checking %s\n", url);
    apiData = http_get(url);
    if (apiData == NULL || apiData->memory == NULL || apiData->size == 0) {
        fprintf(stderr, "Unable to retrieve data from URL %s\n", url);
    } else {
        newQs = se_load(apiData->memory, &numNewQs, page_size);
        free(apiData->memory);
        tracef("Received %d questions\n", numNewQs);
        if (!display_all) {
          if (oldQs != NULL) {
            trace("Checking for updates\n");
            se_check_for_updates(oldQs, numOldQs, newQs, numNewQs);
          }
          se_free_questions(oldQs, numOldQs);
          oldQs = newQs;
          numOldQs = numNewQs;
        } else {
          se_check_for_updates(oldQs, numOldQs, newQs, numNewQs);
          se_free_questions(newQs, numNewQs);
        }
    }

    sleep(60 * poll_rate_mins);
  }

  free(url);
  se_free_questions(oldQs, numOldQs);
  se_free_questions(newQs, numNewQs);
}

int main(int argc, char **argv) {
  int c;
  int display_all = 0;
  int pollrate = 15; // mins
  int pagesize = 10;
  char *tags = "";
  char *site = "stackoverflow";

  static struct option long_options[] =
  {
    {"all",         no_argument, NULL, 'a'},
    {"help",        no_argument, NULL, 'h'},
    {"pagesize",    required_argument, NULL, 'p'},
    {"rate",        required_argument, NULL, 'r'},
    {"site",        required_argument, NULL, 's'},
    {"tagged",      required_argument, NULL, 't'},
    {"version",     no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0}
  };

  while ((c = getopt_long(argc, argv, "ap:r:s:t:v", long_options, NULL)) != -1) {
    switch (c) {
      case 'v':
        version();
        return 0;
      case 'h':
        usage();
        return 0;
      case 'a':
        display_all = 1;
        break;
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
        return 1;
      default:
        abort ();
    }
  }

  tracef("Starting up with rate = %d, page size = %d, site = %s, tags = %s\n", 
    pollrate, pagesize, site, tags);

  watch(pagesize, pollrate, site, tags, display_all);
  return 0;
}

