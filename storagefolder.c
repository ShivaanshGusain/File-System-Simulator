#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storagefolder.h"

#define SAVE_FILE "filesystem.bin"

static void save_folder(FILE* fp, FOLDER* folder) {
    if (!folder) return;
    
    // Save folder info
    fwrite(folder->name, sizeof(folder->name), 1, fp);
    fwrite(folder->relative_path, sizeof(folder->relative_path), 1, fp);
    fwrite(&folder->file_count, sizeof(int), 1, fp);
    fwrite(&folder->folder_count, sizeof(int), 1, fp);
    fwrite(&folder->metadata, sizeof(METADATA), 1, fp);
    
    // Save files in this folder
    FSFile* file = folder->files_head;
    while (file) {
        fwrite(file->name, sizeof(file->name), 1, fp);
        fwrite(file->relative_path, sizeof(file->relative_path), 1, fp);
        fwrite(&file->metadata, sizeof(METADATA), 1, fp);
        file = file->next;
    }
    
    // Save subfolders
    FOLDER* child = folder->first_child;
    while (child) {
        save_folder(fp, child);
        child = child->next_sibling;
    }
}

static FOLDER* load_folder(FILE* fp, FOLDER* parent) {
    FOLDER* folder = (FOLDER*)calloc(1, sizeof(FOLDER));
    
    // Read folder info
    if (fread(folder->name, sizeof(folder->name), 1, fp) != 1) {
        free(folder);
        return NULL;
    }
    fread(folder->relative_path, sizeof(folder->relative_path), 1, fp);
    fread(&folder->file_count, sizeof(int), 1, fp);
    fread(&folder->folder_count, sizeof(int), 1, fp);
    fread(&folder->metadata, sizeof(METADATA), 1, fp);
    
    folder->parent = parent;
    
    // Read files
    for (int i = 0; i < folder->file_count; i++) {
        FSFile* file = (FSFile*)calloc(1, sizeof(FSFile));
        fread(file->name, sizeof(file->name), 1, fp);
        fread(file->relative_path, sizeof(file->relative_path), 1, fp);
        fread(&file->metadata, sizeof(METADATA), 1, fp);
        
        file->parent_folder = folder;
        
        // Add to list
        if (!folder->files_head) {
            folder->files_head = file;
        } else {
            folder->files_tail->next = file;
            file->prev = folder->files_tail;
        }
        folder->files_tail = file;
    }
    
    // Read subfolders
    for (int i = 0; i < folder->folder_count; i++) {
        FOLDER* child = load_folder(fp, folder);
        if (child) {
            // Add to list
            if (!folder->first_child) {
                folder->first_child = child;
            } else {
                folder->last_child->next_sibling = child;
                child->prev_sibling = folder->last_child;
            }
            folder->last_child = child;
        }
    }
    
    return folder;
}

void save_filesystem(FOLDER* root) {
    if (!root) return;
    
    FILE* fp = fopen(SAVE_FILE, "wb");
    if (!fp) {
        printf("Error: Cannot save filesystem.\n");
        return;
    }
    
    save_folder(fp, root);
    fclose(fp);
}

FOLDER* load_filesystem(void) {
    FILE* fp = fopen(SAVE_FILE, "rb");
    if (!fp) {
        return NULL;
    }
    
    FOLDER* root = load_folder(fp, NULL);
    fclose(fp);
    return root;
}