#ifndef STORAGEFOLDER_H
#define STORAGEFOLDER_H

#include "structure.h"

void save_filesystem(FOLDER* root);
FOLDER* load_filesystem(void);

#endif