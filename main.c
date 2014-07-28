// JSON parser code from http://linuxprograms.wordpress.com/2010/08/19/json_parser_json-c/
#include <curl/curl.h>
#include <json/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "http.h"
#include "se-api.h"
#include "util.h"

#define MAX_UPDATE_SIZE 10

// /*printing the value corresponding to boolean, double, integer and strings*/
// void print_json_value(json_object *jobj){
//   enum json_type type;
//   printf("type: %d\n", type);
//   type = json_object_get_type(jobj); /*Getting the type of the json object*/
//   switch (type) {
//     case json_type_boolean: printf("json_type_booleann");
//                          printf("value: %sn", json_object_get_boolean(jobj)? "true": "false");
//                          break;
//     case json_type_double: printf("json_type_doublen");
//                         printf("          value: %lfn", json_object_get_double(jobj));
//                          break;
//     case json_type_int: printf("json_type_intn");
//                         printf("          value: %dn", json_object_get_int(jobj));
//                          break;
//     case json_type_string: printf("json_type_stringn");
//                          printf("          value: %sn", json_object_get_string(jobj));
//                          break;
//   }
// 
// }
// 
// void json_parse_array( json_object *jobj, char *key) {
//   void json_parse(json_object * jobj); /*Forward Declaration*/
//   enum json_type type;
// 
//   json_object *jarray = jobj; /*Simply get the array*/
//   if(key) {
//     jarray = json_object_object_get(jobj, key); /*Getting the array if it is a key value pair*/
//   }
// 
//   int arraylen = json_object_array_length(jarray); /*Getting the length of the array*/
//   printf("Array Length: %dn",arraylen);
//   int i;
//   json_object * jvalue;
// 
//   for (i=0; i< arraylen; i++){
//     jvalue = json_object_array_get_idx(jarray, i); /*Getting the array element at position i*/
//     type = json_object_get_type(jvalue);
//     if (type == json_type_array) {
//       json_parse_array(jvalue, NULL);
//     }
//     else if (type != json_type_object) {
//       printf("value[%d]: ",i);
//       print_json_value(jvalue);
//     }
//     else {
//       json_parse(jvalue);
//     }
//   }
// }
// 
// /*Parsing the json object*/
// void json_parse(json_object * jobj) {
//   enum json_type type;
//   json_object_object_foreach(jobj, key, val) { /*Passing through every array element*/
//     printf("type: %d\n", type);
//     type = json_object_get_type(val);
//     switch (type) {
//       case json_type_boolean: 
//       case json_type_double: 
//       case json_type_int: 
//       case json_type_string: 
//         print_json_value(val);
//         break; 
//       case json_type_object: 
//         printf("json_type_objectn");
//         jobj = json_object_object_get(jobj, key);
//         json_parse(jobj); 
//         break;
//       case json_type_array: 
//         printf("type: json_type_array, ");
//         json_parse_array(jobj, key);
//         break;
//     }
//   }
// } 

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
int main() {
  int poll_rate_mins = 15;
  char url[] = "http://api.stackexchange.com/2.2/questions?page=1&pagesize=10&order=desc&min=10&sort=activity&tagged=a-song-of-ice-and-fire&site=scifi";
  int numOldQs = 0;
  int numNewQs = 0;

// Test code:
//  struct SeQuestion **oldQs = 
//    se_load(getFileContents("results.json", NULL), &numOldQs);
//  struct SeQuestion **newQs = 
//    se_load(getFileContents("results2.json", NULL), &numNewQs);
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
  return 0;
}
