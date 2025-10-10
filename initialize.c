#include<stdio.h>
#include<stdlib.h>
#include "filesystem.h"
#include "dir_ops.h"

FOLDER* root_folder = NULL;
FOLDER* current_folder = NULL;

void init_filesystem() {
    root_folder = dir_new("/", NULL);
    current_folder = root_folder;
}

