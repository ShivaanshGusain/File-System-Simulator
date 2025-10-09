#ifndef FILE_OPS_H
#define FILE_OPS_H
#include "structure.h"
#include "dir_ops.h"

// File operations
FSFile* file_new(const char* name, FOLDER* parent, size_t size);
int file_delete(FOLDER* folder, const char* name);
int file_rename(FOLDER* folder, const char* old_name, const char* new_name);
int file_move(FSFile* file, FOLDER* new_parent);
void file_info(FSFile* file);
void file_list(FOLDER* folder);

#endif
