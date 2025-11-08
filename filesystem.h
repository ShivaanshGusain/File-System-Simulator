#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "structure.h"

extern FOLDER* root_folder;
extern FOLDER* current_folder;

void init_filesystem();
void init_filesystem();
FOLDER* get_root();
char* get_current_path();
int change_directory(const char* path);
int count_all_items(FOLDER* folder);


#endif
