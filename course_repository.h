#pragma once

#include "stdlib.h"
#include "course.h"
#include "group.h"

struct CourseIndex {
    int course_id;
    int address;
};

struct Course* get_m(int course_id);
struct Course* get_s(int course_id, int group_id);
int del_m(int course_id);
int del_s(int course_id, int group_id);
int update_m(struct Course course);
int update_s(int course_id, struct Group group);
int insert_m(struct Course course);
int insert_s(int course_id, struct Group group);
size_t size_m();
size_t size_s(int course_id);
int ut_m();
int ut_s();