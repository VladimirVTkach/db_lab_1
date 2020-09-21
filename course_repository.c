#include <stdio.h>
#include "course_repository.h"

#define MAIN_FILE_PATH "../courses.fl"
#define INDEX_FILE_PATH "../courses.ind"

size_t get_file_size(char *path);

int compare_index(const void *lhs, const void *rhs);

struct Course *get_m(int course_id);

struct Course *get_s(int course_id, int group_id);

int del_m(int course_id);

int del_s(int course_id, int group_id);

int update_m(int course_id, struct Course course);

int update_s(int course_id, int group_id, struct Group group);

int insert_m(struct Course course) {
    size_t main_file_size = get_file_size(MAIN_FILE_PATH);
    struct CourseIndex index = {course.course_id, main_file_size};

    FILE *index_file = fopen(INDEX_FILE_PATH, "r+");
    if (index_file == 0) {
        return -1;
    }

    size_t index_file_size = get_file_size(INDEX_FILE_PATH);
    size_t index_structure_size = sizeof(struct CourseIndex);

    size_t index_buffer_size = index_file_size + index_structure_size;
    struct CourseIndex *index_buffer = malloc(index_buffer_size);
    size_t index_items_read_cnt = fread(index_buffer,
                                        index_structure_size,
                                        index_file_size / index_structure_size,
                                        index_file);
    if (index_items_read_cnt < index_file_size / index_structure_size) {
        printf("error while reading index file occurred");
        free(index_buffer);
        fclose(index_file);
        return -1;
    }
    *(index_buffer + index_file_size / index_structure_size) = index;
    qsort(index_buffer,
          index_buffer_size / index_structure_size,
          index_structure_size,
          compare_index);

    size_t written_index_items_cnt = fwrite(index_buffer,
                                            index_structure_size,
                                            index_buffer_size / index_structure_size,
                                            index_file);
    if (written_index_items_cnt < index_buffer_size / index_structure_size) {
        printf("error while writing index occurred");
        free(index_buffer);
        fclose(index_file);
        return -1;
    }
    free(index_buffer);
    fclose(index_file);

    FILE *main_file = fopen(MAIN_FILE_PATH, "a");
    if (main_file == 0) {
        printf("main file not found");
        return -1;
    }
    size_t written_data_items_cnt = fwrite(&course, sizeof(course), 1, main_file);
    if (written_data_items_cnt != 1) {
        printf("error while writing data occurred");
        fclose(main_file);
        return -1;
    }
    fclose(main_file);
    return 0;
}

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

int compare_index(const void *lhs, const void *rhs) {
    struct CourseIndex *lhs_index = (struct CourseIndex *) lhs;
    struct CourseIndex *rhs_index = (struct CourseIndex *) rhs;

    return lhs_index->course_id - rhs_index->course_id;
}
