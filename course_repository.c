#include <stdio.h>
#include "course_repository.h"

#define COURSES_FILE_PATH "../courses.fl"
#define COURSES_INDEX_FILE_PATH "../courses.ind"
#define GROUPS_FILE_PATH "../groups.fl"

size_t COURSES_STRUCTURE_SIZE = sizeof(struct Course);
size_t COURSES_INDEX_STRUCTURE_SIZE = sizeof(struct CourseIndex);
size_t GROUP_STRUCTURE_SIZE = sizeof(struct Group);

size_t get_file_size(char *path);

int compare_index(const void *lhs, const void *rhs);

struct Course *get_m(int course_id) {
    FILE *courses_index_file = fopen(COURSES_INDEX_FILE_PATH, "r");
    if (courses_index_file == 0) {
        return 0;
    }

    size_t courses_index_file_size = get_file_size(COURSES_INDEX_FILE_PATH);

    struct CourseIndex *index_buffer = malloc(courses_index_file_size);
    size_t index_items_read_cnt = fread(index_buffer, COURSES_INDEX_STRUCTURE_SIZE, courses_index_file_size,
                                        courses_index_file);
    if (index_items_read_cnt < COURSES_INDEX_STRUCTURE_SIZE / courses_index_file_size) {
        printf("error while reading courses index file occurred");
        free(index_buffer);
        fclose(courses_index_file);
        return 0;
    }
    fclose(courses_index_file);

    struct CourseIndex key = {course_id, -1};
    struct CourseIndex *index = bsearch(&key,
                                        index_buffer,
                                        courses_index_file_size / COURSES_INDEX_STRUCTURE_SIZE,
                                        COURSES_INDEX_STRUCTURE_SIZE,
                                        compare_index);
    if (index == 0) {
        free(index_buffer);
        return 0;
    }
    free(index_buffer);

    long courses_file_address = index->address;

    FILE *courses_file = fopen(COURSES_FILE_PATH, "r");
    if (courses_file == 0) {
        return 0;
    }

    fseek(courses_file, courses_file_address, SEEK_SET);

    struct Course *course = malloc(COURSES_STRUCTURE_SIZE);
    size_t data_items_read_cnt = fread(course, COURSES_STRUCTURE_SIZE, 1, courses_file);
    if (data_items_read_cnt != 1) {
        printf("error while reading courses file occurred");
        fclose(courses_file);
        free(course);
        return 0;
    }

    if (course->is_deleted == 1) {
        fclose(courses_file);
        return 0;
    }

    fclose(courses_file);
    return course;
}

struct Group *get_s(int course_id, int group_id) {
    struct Course *course = get_m(course_id);

    if (course != 0) {
        int group_address = course->group_address;
        free(course);

        FILE *groups_file = fopen(GROUPS_FILE_PATH, "r+");
        if (groups_file == 0) {
            return 0;
        }

        struct Group *group = malloc(GROUP_STRUCTURE_SIZE);
        while (group_address != -1) {
            fseek(groups_file, group_address, SEEK_SET);
            size_t group_items_read_cnt = fread(group, GROUP_STRUCTURE_SIZE, 1, groups_file);
            if (group_items_read_cnt != 1) {
                printf("error while reading groups file occurred");
                fclose(groups_file);
                return 0;
            }

            if (group->group_id == group_id && group->is_deleted == 0) {
                fclose(groups_file);
                return group;
            }

            group_address = group->next_group_address;
        }

        if (group->group_id == group_id && group->is_deleted == 0) {
            fclose(groups_file);
            return group;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

int del_m(int course_id) {
    struct Course *course = get_m(course_id);

    if (course == 0) {
        printf("course with such id already deleted");
        return -1;
    }
    int group_address = course->group_address;

    FILE *groups_file = fopen(GROUPS_FILE_PATH, "r");
    if (groups_file == 0) {
        return -1;
    }

    struct Group *group = malloc(GROUP_STRUCTURE_SIZE);
    while (group_address != -1) {
        fseek(groups_file, group_address, SEEK_SET);
        size_t group_items_read_cnt = fread(group, GROUP_STRUCTURE_SIZE, 1, groups_file);
        if (group_items_read_cnt != 1) {
            printf("error while reading groups file occurred");
            fclose(groups_file);
            free(group);
            return -1;
        }

        del_s(course_id, group->group_id);
        group_address = group->next_group_address;
    }

    course->is_deleted = 1;
    return update_m(*course);
}

int del_s(int course_id, int group_id) {
    struct Group *group = get_s(course_id, group_id);
    if (group != 0) {
        group->is_deleted = 1;
        return update_s(course_id, *group);
    }
    return -1;
}

int update_m(struct Course course) {
    struct Course *previous_val = get_m(course.course_id);

    if (previous_val == 0) {
        printf("course with such id doesn't exits");
        return -1;
    }

    FILE *courses_index_file = fopen(COURSES_INDEX_FILE_PATH, "r");
    if (courses_index_file == 0) {
        return -1;
    }

    size_t courses_index_file_size = get_file_size(COURSES_INDEX_FILE_PATH);

    struct CourseIndex *index_buffer = malloc(courses_index_file_size);
    size_t index_items_read_cnt = fread(index_buffer, COURSES_INDEX_STRUCTURE_SIZE, courses_index_file_size,
                                        courses_index_file);
    if (index_items_read_cnt < COURSES_INDEX_STRUCTURE_SIZE / courses_index_file_size) {
        printf("error while reading courses index file occurred");
        free(index_buffer);
        fclose(courses_index_file);
        return -1;
    }
    fclose(courses_index_file);

    struct CourseIndex key = {course.course_id, -1};
    struct CourseIndex *index = bsearch(&key,
                                        index_buffer,
                                        courses_index_file_size / COURSES_INDEX_STRUCTURE_SIZE,
                                        COURSES_INDEX_STRUCTURE_SIZE,
                                        compare_index);
    if (index == 0) {
        printf("course record you're trying to update doesn't exits");
        free(index_buffer);
        return -1;
    }

    long courses_file_address = index->address;
    free(index_buffer);

    FILE *courses_file = fopen(COURSES_FILE_PATH, "r+");
    if (courses_file == 0) {
        return -1;
    }
    fseek(courses_file, courses_file_address, SEEK_SET);

    size_t written_course_items_cnt = fwrite(&course,
                                             COURSES_STRUCTURE_SIZE,
                                             1,
                                             courses_file);
    if (written_course_items_cnt != 1) {
        printf("error while writing to courses file occurred");
        fclose(courses_file);
        return -1;
    }
    fclose(courses_file);
    return 0;
}

int update_s(int course_id, struct Group group) {
    struct Course *course = get_m(course_id);

    if (course == 0) {
        printf("course with such id doesn't exits");
        return -1;
    }
    int group_address = course->group_address;

    FILE *groups_file = fopen(GROUPS_FILE_PATH, "r+");
    if (groups_file == 0) {
        return 0;
    }

    struct Group *found_group = malloc(GROUP_STRUCTURE_SIZE);
    while (group_address != -1) {
        fseek(groups_file, group_address, SEEK_SET);
        size_t group_items_read_cnt = fread(found_group, GROUP_STRUCTURE_SIZE, 1, groups_file);
        if (group_items_read_cnt != 1) {
            printf("error while reading groups file occurred");
            fclose(groups_file);
            free(found_group);
            return -1;
        }

        if (found_group->group_id == group.group_id && found_group->is_deleted == 0) {
            fseek(groups_file, group_address, SEEK_SET);

            size_t written_group_items_cnt = fwrite(&group,
                                                    GROUP_STRUCTURE_SIZE,
                                                    1,
                                                    groups_file);
            if (written_group_items_cnt != 1) {
                printf("error while writing to groups file occurred");
                fclose(groups_file);
                free(found_group);
                return -1;
            }

            fclose(groups_file);
            free(found_group);
            return 0;
        }

        group_address = found_group->next_group_address;
    }

    if (found_group->group_id == group.group_id && found_group->is_deleted == 0) {
        fseek(groups_file, group_address, SEEK_SET);

        size_t written_group_items_cnt = fwrite(&group,
                                                GROUP_STRUCTURE_SIZE,
                                                1,
                                                groups_file);
        if (written_group_items_cnt != 1) {
            printf("error while writing to groups file occurred");
            fclose(groups_file);
            free(found_group);
            return -1;
        }

        fclose(groups_file);
        free(found_group);
        return 0;
    } else {
        printf("group with such id doesn't exits");
        fclose(groups_file);
        free(found_group);
        return -1;
    }
}

int insert_m(struct Course course) {
    size_t courses_file_size = get_file_size(COURSES_FILE_PATH);
    struct CourseIndex index = {course.course_id, courses_file_size};

    FILE *courses_index_file = fopen(COURSES_INDEX_FILE_PATH, "r+");
    if (courses_index_file == 0) {
        return -1;
    }

    size_t courses_index_file_size = get_file_size(COURSES_INDEX_FILE_PATH);

    size_t index_buffer_size = courses_index_file_size + COURSES_INDEX_STRUCTURE_SIZE;
    struct CourseIndex *index_buffer = malloc(index_buffer_size);
    size_t index_items_read_cnt = fread(index_buffer,
                                        COURSES_INDEX_STRUCTURE_SIZE,
                                        courses_index_file_size / COURSES_INDEX_STRUCTURE_SIZE,
                                        courses_index_file);
    if (index_items_read_cnt < courses_index_file_size / COURSES_INDEX_STRUCTURE_SIZE) {
        printf("error while reading courses index file occurred");
        free(index_buffer);
        fclose(courses_index_file);
        return -1;
    }
    *(index_buffer + courses_index_file_size / COURSES_INDEX_STRUCTURE_SIZE) = index;
    qsort(index_buffer,
          index_buffer_size / COURSES_INDEX_STRUCTURE_SIZE,
          COURSES_INDEX_STRUCTURE_SIZE,
          compare_index);

    size_t written_index_items_cnt = fwrite(index_buffer,
                                            COURSES_INDEX_STRUCTURE_SIZE,
                                            index_buffer_size / COURSES_INDEX_STRUCTURE_SIZE,
                                            courses_index_file);
    if (written_index_items_cnt < index_buffer_size / COURSES_INDEX_STRUCTURE_SIZE) {
        printf("error while writing to courses index file occurred");
        free(index_buffer);
        fclose(courses_index_file);
        return -1;
    }
    free(index_buffer);
    fclose(courses_index_file);

    FILE *courses_file = fopen(COURSES_FILE_PATH, "a");
    if (courses_file == 0) {
        return -1;
    }
    size_t written_data_items_cnt = fwrite(&course, COURSES_STRUCTURE_SIZE, 1, courses_file);
    if (written_data_items_cnt != 1) {
        printf("error while writing to courses file occurred");
        fclose(courses_file);
        return -1;
    }
    fclose(courses_file);
    return 0;
}

int insert_s(int course_id, struct Group group) {
    struct Course *course = get_m(course_id);

    if (course == 0) {
        printf("course with such id doesn't exist");
        return -1;
    }

    FILE *groups_file = fopen(GROUPS_FILE_PATH, "a");
    if (groups_file == 0) {
        return -1;
    }

    size_t groups_file_size = get_file_size(GROUPS_FILE_PATH);

    group.next_group_address = course->group_address;

    size_t written_group_items_cn = fwrite(&group, GROUP_STRUCTURE_SIZE, 1, groups_file);
    if (written_group_items_cn != 1) {
        printf("error while writing to groups file occurred");
        fclose(groups_file);
        return -1;
    }
    fclose(groups_file);

    course->group_address = groups_file_size;
    return update_m(*course);
}

size_t count_m() {
    FILE *courses_file = fopen(COURSES_FILE_PATH, "r");
    if (courses_file == 0) {
        return -1;
    }
    struct Course *course = malloc(COURSES_STRUCTURE_SIZE);

    int items_cnt = 0;
    while (fread(course, COURSES_STRUCTURE_SIZE, 1, courses_file) == 1) {
        if (course->is_deleted == 0) {
            items_cnt++;
        }
    }
    fclose(courses_file);
    free(course);
    return items_cnt;
}

size_t count_all_s() {
    FILE *courses_file = fopen(COURSES_FILE_PATH, "r");
    if (courses_file == 0) {
        return -1;
    }
    struct Course *course = malloc(COURSES_STRUCTURE_SIZE);

    int items_cnt = 0;
    while (fread(course, COURSES_STRUCTURE_SIZE, 1, courses_file) == 1) {
        if (course->is_deleted == 0) {
            items_cnt += count_s(course->course_id);
        }
    }
    fclose(courses_file);
    free(course);
    return items_cnt;
}

size_t count_s(int course_id) {
    struct Course *course = get_m(course_id);

    if (course == 0) {
        printf("course with such id already deleted");
        return -1;
    }
    int group_address = course->group_address;
    free(course);

    FILE *groups_file = fopen(GROUPS_FILE_PATH, "r");
    if (groups_file == 0) {
        return -1;
    }

    int groups_cnt = 0;
    struct Group *group = malloc(COURSES_STRUCTURE_SIZE);
    while (group_address != -1) {
        fseek(groups_file, group_address, SEEK_SET);
        size_t group_items_read_cnt = fread(group, COURSES_STRUCTURE_SIZE, 1, groups_file);
        if (group_items_read_cnt != 1) {
            printf("error while reading groups file occurred");
            fclose(groups_file);
            free(group);
            return -1;
        }
        group_address = group->next_group_address;
        if (group->is_deleted == 0) {
            groups_cnt++;
        }
    }

    fclose(groups_file);
    free(group);
    return groups_cnt;
}

size_t get_file_size(char *path) {
    FILE *file = fopen(path, "r");
    if (file == 0) {
        return -1;
    }
    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    fclose(file);
    return size;
}

int compare_index(const void *lhs, const void *rhs) {
    struct CourseIndex *lhs_index = (struct CourseIndex *) lhs;
    struct CourseIndex *rhs_index = (struct CourseIndex *) rhs;

    return lhs_index->course_id - rhs_index->course_id;
}
