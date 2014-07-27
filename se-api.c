
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

