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

#define MAX_UPDATE_SIZE 10

// Based on JSON parser code from 
// http://linuxprograms.wordpress.com/2010/08/19/json_parser_json-c/
struct SeQuestion **parse_se(json_object *jobj, int *result_size) {
  struct SeQuestion **results = 
    (struct SeQuestion **)calloc(sizeof(struct SeQuestion *), MAX_UPDATE_SIZE);
  int result_idx = 0;
  enum json_type type = json_object_get_type(jobj);

  if (type == json_type_object) {
    json_object *items = json_object_object_get(jobj, "items");
    type = json_object_get_type(items);

    if (type == json_type_array) {
      int len = json_object_array_length(items);
      for (int i = 0; i < len; i++) {
        json_object *jval = json_object_array_get_idx(items, i);
        if (json_object_get_type(jval) == json_type_object) {
          json_object *jqid = json_object_object_get(jval, "question_id");
          json_object *jlact = json_object_object_get(jval, "last_activity_date");
          json_object *jtitle = json_object_object_get(jval, "title");
          json_object *jlink = json_object_object_get(jval, "link");
          json_object *jscore = json_object_object_get(jval, "score");
          json_object *jnum_ans = json_object_object_get(jval, "answer_count");
          json_object *jnum_views = json_object_object_get(jval, "view_count");
          struct SeQuestion *q = se_new_question(
              json_object_get_int(jqid)
            , json_object_get_int(jscore)
            , json_object_get_int(jnum_ans)
            , json_object_get_int(jnum_views)
            , json_object_get_int(jlact)
            , json_object_get_string(jtitle)
            , json_object_get_string(jlink)
          ); 
          //se_print_question(q);
          results[result_idx++] = q;
        }
      }
    } else {
      printf("Unexpected json type\n");
    }
  }

  *result_size = result_idx;
  return results;
}

struct SeQuestion **se_load(char *string, int *numQs) {
  json_object * jobj = json_tokener_parse(string);     
  struct SeQuestion **newQs = parse_se(jobj, numQs);
  json_object_put(jobj);
  free(string);
  return newQs;
}

// TODO: include command line arguments.
// could specify API string, poll rate (in minutes, no less than 1) from cli
// if none are specified, maybe read from a .rc file?
// otherwise either give up or use default (not sure default search makes any sense, though. probably better to print link to the SE API query page)
void watch() {
  int poll_rate_mins = 15;
  char url[] = "http://api.stackexchange.com/2.2/questions?page=1&pagesize=10&order=desc&min=10&sort=activity&tagged=a-song-of-ice-and-fire&site=scifi";
  int numOldQs = 0;
  int numNewQs = 0;

// TODO: move test code to another file, and add to makefile
// Test code:
//  struct SeQuestion **oldQs = 
//    se_load(getFileContents("results.json", NULL), &numOldQs);
//  struct SeQuestion **newQs = 
//    se_load(getFileContents("results2.json", NULL), &numNewQs);
// TODO: let this return a value so we can test for it
//  se_check_for_updates(oldQs, numOldQs, newQs, numNewQs);

  struct SeQuestion **oldQs = NULL;
  struct SeQuestion **newQs = NULL;
  while(1) {
    tracef("Woke up, checking %s\n", url);
    struct MemoryStruct *apiData = http_get(url);
    newQs = se_load(apiData->memory, &numNewQs);
    tracef("Received %d questions\n", numNewQs);
    if (oldQs != NULL) {
      trace("Checking for updates\n");
      se_check_for_updates(oldQs, numOldQs, newQs, numNewQs);
    }
    se_free_questions(oldQs, numOldQs);
    oldQs = newQs;
    numOldQs = numNewQs;

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

  while ((c = getopt (argc, argv, "abt:")) != -1) {
    switch (c) {
//      case 'a':
//        aflag = 1;
//        break;
//      case 'b':
//        bflag = 1;
//        break;
      case 't':
        tags = optarg;
        break;
      case '?':
        if (optopt == 't')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
    }
  }

  printf("Tags = %s\n", tags);

// TODO: pass args to core function:
  //watch();
  return 0;
}

