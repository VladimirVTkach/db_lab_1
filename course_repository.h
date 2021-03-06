#pragma once

#include "stdlib.h"
#include "course.h"
#include "group.h"

struct CourseIndex {
    int course_id;
    int address;
};

struct Course* get_m(int course_id);
struct Group* get_s(int course_id, int group_id);
int del_m(int course_id);
int del_s(int course_id, int group_id);
int update_m(struct Course course);
int update_s(int course_id, struct Group group);
int insert_m(struct Course course);
int insert_s(int course_id, struct Group group);
size_t count_m();
size_t count_all_s();
size_t count_s(int course_id);