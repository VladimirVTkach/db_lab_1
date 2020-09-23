#include <stdio.h>
#include "course.h"
#include "group.h"
#include "course_repository.h"

int main() {
    struct Course first_course = {1, 1, -1};
    insert_m(first_course);
    struct Course second_course = {2, 1, -1};
    insert_m(second_course);

    struct Group first_group = {1, "K-18", -1};
    struct Group second_group = {2, "K-19", -1};
    insert_s(1, first_group);
    insert_s(1, second_group);

    struct Group third_group = {3, "K-28", -1};
    struct Group fourth_group = {4, "K-29", -1};
    insert_s(2, third_group);
    insert_s(2, fourth_group);

    struct Group *foundGroup = get_s(2, 4);
    if (foundGroup != 0) {
        printf("group_id: %d\nname: %s\n",
               foundGroup->group_id,
               foundGroup->name);
    } else {
        printf("not found");
    }
    return 0;
}
