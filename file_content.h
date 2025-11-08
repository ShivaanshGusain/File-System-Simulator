#ifndef FILE_CONTENT_H
#define FILE_CONTENT_H

#include "structure.h"

void write_text_to_file(FSFile* file);
void read_text_from_file(FSFile* file);
void save_file_contents(void);
void load_file_contents(void);

#endif