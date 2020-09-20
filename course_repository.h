#include "course.h"
#include "group.h"

struct CourseIndex {
    int course_id;
    int address;
};

struct Course* get_m(int course_id);
struct Course* get_s(int course_id, int group_id);
void del_m(int course_id);
void del_s(int course_id, int group_id);
void update_m(int course_id, struct Course course);
void update_s(int course_id, int group_id, struct Group group);
void insert_m(struct Course course);
void insert_s(int course_id, struct Group group);
int size_m();
int size_s(int course_id);
int ut_m();
int ut_s();