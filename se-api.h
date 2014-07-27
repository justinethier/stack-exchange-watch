#ifndef __SE_API_H__
#define __SE_API_H__

struct SeQuestion {
  int id;
  int score;
  int answers;
  int views;
  int last_activity_date;
  char *title;
  char *link;
};

void se_print_question(struct SeQuestion *q);
struct SeQuestion *se_new_question(int id, int score, int answers,
  int views, int last_activity_date, const char *title, const char *link);
void se_free_question(struct SeQuestion *q);
void se_free_questions(struct SeQuestion **qs, int numQs);
struct SeQuestion **se_load(char *string, int *numQs);
int se_has_update(struct SeQuestion **old, int numOld, 
                  struct SeQuestion *new);
void se_check_for_updates(struct SeQuestion **old, int numOld, 
                          struct SeQuestion **new, int numNew);

#endif
