#ifndef DIR_OPS_H
#define DIR_OPS_H

#include "structure.h"

// Directory operation function declarations
FOLDER* dir_new(char *name, FOLDER *parent);
void dir_add_child(FOLDER *parent, FOLDER *child);
FOLDER* find_dir(FOLDER *root, char *name);
void mkdir_dir(FOLDER *root, char *path);
void move_folder(FOLDER *root, char *src_name, char *dest_name);
void rename_folder(FOLDER *root, char *old_name, char *new_name);
void rmdir_dir(FOLDER *root, char *name);
//void rmdir_recursive(FOLDER *root, char *name);


// like Linux cd
int change_directory(const char* path);
char* get_current_path(void);


// Display functions
void print(FOLDER *dir, int level);
void print_directory(FOLDER *root);
void print_path(FOLDER *dir);
void print_dir_info(FOLDER *dir);

#endif