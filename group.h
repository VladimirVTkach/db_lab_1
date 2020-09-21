#pragma once

struct Group {
    int group_id;
    char name[10];
    int next_group_ref;
};