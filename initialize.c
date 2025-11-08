#include <stdio.h>
#include <string.h>
#include "filesystem.h"
#include "dir_ops.h"
#include "resolve.h" 

FOLDER* root_folder = NULL;
FOLDER* current_folder = NULL;

void init_filesystem() {
    root_folder = dir_new("/", NULL);
    strcpy(root_folder->relative_path, "/");
    current_folder = root_folder;
}

FOLDER* get_root() {
    return root_folder;
}


int count_all_items(FOLDER* folder) {
    if (!folder) return 0;
    int count = 1 + folder->file_count;
    FOLDER* child = folder->first_child;
    while (child) {
        count += count_all_items(child);
        child = child->next_sibling;
    }
    return count;
}