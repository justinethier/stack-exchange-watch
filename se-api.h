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
int se_has_update(struct SeQuestion **old, int numOld, 
                  struct SeQuestion *new);
void se_check_for_updates(struct SeQuestion **old, int numOld, 
                          struct SeQuestion **new, int numNew);
struct SeQuestion **se_parse_questions(json_object *jobj, int *result_size,
                                       int page_size);
struct SeQuestion **se_load(char *string, int *numQs, int page_size);

char *se_api_url(int page_size, const char *site, const char *tags);

#endif
