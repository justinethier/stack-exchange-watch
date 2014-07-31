
#include <json/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "se-api.h"
#include "util.h"

void se_print_question(struct SeQuestion *q) {
 printf("%2d %2d %4d - %s\n%s\n\n", 
     q->score
   , q->answers
   , q->views
   , q->title
   , q->link); 
}

struct SeQuestion *se_new_question(int id, int score, int answers,
  int views, int last_activity_date, const char *title, const char *link) {
  struct SeQuestion *q = 
    (struct SeQuestion *)calloc(sizeof(struct SeQuestion), 1);
  q->id =                 id;
  q->score =              score;
  q->answers =            answers;
  q->views =              views;
  q->last_activity_date = last_activity_date;
  q->title = _strdup(title);
  q->link = _strdup(link);
  return q;
}

void se_free_question(struct SeQuestion *q){
  if (q) {
    if (q->title) free(q->title);
    if (q->link) free(q->link);
    free(q);
  }
}

void se_free_questions(struct SeQuestion **qs, int numQs) {
  if (qs) {
    for (int i = 0; i < numQs; i++) {
      se_free_question(qs[i]);
    }
    free(qs);
  }
}

int se_has_update(struct SeQuestion **old, int numOld, 
                  struct SeQuestion *new) {
  for (int i = 0; i < numOld; i++) {
    if (new->id == old[i]->id) {
      return new->last_activity_date > old[i]->last_activity_date;
    }
  }

  // New question since last set
  return 1;
}

void se_check_for_updates(struct SeQuestion **old, int numOld, 
                          struct SeQuestion **new, int numNew) {
  int updated = 0;
  for (int i = 0; i < numNew; i++) {
    if (se_has_update(old, numOld, new[i])) {
      se_print_question(new[i]);
      updated++;
    }
  }

  if (updated){
    time_t now = time(NULL);
    struct tm * p = localtime(&now);
    char s[1000];
    strftime(s, 1000, "%A, %B %d %Y at %H:%M:%S", p);
    printf("%d update(s) detected on %s\n", updated, s);
  }
}

// Based on JSON parser code from 
// http://linuxprograms.wordpress.com/2010/08/19/json_parser_json-c/
struct SeQuestion **se_parse_questions(json_object *jobj, int *result_size, 
                                       int page_size) {
  struct SeQuestion **results = 
    (struct SeQuestion **)calloc(sizeof(struct SeQuestion *), page_size);
  int result_idx = 0;
  enum json_type type = json_object_get_type(jobj);

  if (type == json_type_object) {
    json_object *items = json_object_object_get(jobj, "items");

    if (items) {
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

      // Check API quota
      json_object *quotaRem = json_object_object_get(jobj, "quota_remaining");
      if (quotaRem) {
          int remaining = json_object_get_int(quotaRem);
          if (remaining < 500) {
            json_object *quotaMax = json_object_object_get(jobj, "quota_max");
            fprintf(stderr, "Warning, remaining quota is %d of %d\n" 
              , remaining
              , json_object_get_int(quotaMax));
          }
      }
    } else {
      // No items, check for an error
      json_object *error_id = json_object_object_get(jobj, "error_id");
      if (error_id) {
        json_object *error_msg = json_object_object_get(jobj, "error_message");
        json_object *error_name = json_object_object_get(jobj, "error_name");
        fprintf(stderr, "Error %d [%s]: %s\n"
          , json_object_get_int(error_id)
          , json_object_get_string(error_name)
          , json_object_get_string(error_msg));
        exit(1);
      }
    }
  }

  *result_size = result_idx;
  return results;
}

struct SeQuestion **se_load(char *string, int *numQs, int page_size) {
  json_object * jobj = json_tokener_parse(string);     
  struct SeQuestion **newQs = se_parse_questions(jobj, numQs, page_size);
  json_object_put(jobj);
  free(string);
  return newQs;
}

