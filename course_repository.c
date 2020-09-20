#include <stdio.h>
#include "course_repository.h"

#define MAIN_FILE_PATH "courses.fl"
#define INDEX_FILE_PATH "courses.ind"

size_t get_file_size(char *path);

struct Course *get_m(int course_id);

struct Course *get_s(int course_id, int group_id);

int del_m(int course_id);

int del_s(int course_id, int group_id);

int update_m(int course_id, struct Course course);

int update_s(int course_id, int group_id, struct Group group);

int insert_m(struct Course course);

int insert_s(int course_id, struct Group group);

size_t size_m() {
    return get_file_size(MAIN_FILE_PATH) / sizeof(struct Course);
}

size_t size_s(int course_id);

int ut_m();

int ut_s();

size_t get_file_size(char *path) {
    FILE *file = fopen(path, "r");
    if (file == 0) {
        printf("file not found");
        return -1;
    }
    fseek(file, 0L, SEEK_END);
    return ftell(file);
}

