#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "file_ops.h"

// Create new file
FSFile* file_new(const char* name, FOLDER* parent, size_t size) {
    if (!parent) {
        printf("Error: Parent folder is NULL.\n");
        return NULL;
    }

    FSFile* new_file = (FSFile*)malloc(sizeof(FSFile));
    if (!new_file) {
        printf("Error: Memory allocation failed for file.\n");
        return NULL;
    }

    strncpy(new_file->name, name, 99);
    new_file->name[99] = '\0';
    new_file->parent_folder = parent;
    new_file->prev = parent->files_tail;
    new_file->next = NULL;
    new_file->metadata.created_time = time(NULL);
    new_file->metadata.modified_time = new_file->metadata.created_time;
    new_file->metadata.size = size;

    snprintf(new_file->relative_path, 500, "%s/%s", parent->relative_path, name);

    if (parent->files_tail) parent->files_tail->next = new_file;
    else parent->files_head = new_file;
    parent->files_tail = new_file;
    parent->file_count++;

    printf("File '%s' created successfully in folder '%s'.\n", name, parent->name);
    return new_file;
}

// Delete file by name
int file_delete(FOLDER* folder, const char* name) {
    if (!folder) {
        printf("Error: Folder is NULL.\n");
        return 0;
    }

    FSFile* curr = folder->files_head;
    while (curr && strcmp(curr->name, name) != 0) curr = curr->next;
    if (!curr) {
        printf("Error: File '%s' not found in folder '%s'.\n", name, folder->name);
        return 0;
    }

    if (curr->prev) curr->prev->next = curr->next;
    else folder->files_head = curr->next;
    if (curr->next) curr->next->prev = curr->prev;
    else folder->files_tail = curr->prev;

    free(curr);
    folder->file_count--;
    printf("File '%s' deleted successfully from folder '%s'.\n", name, folder->name);
    return 1;
}

// Rename file
int file_rename(FOLDER* folder, const char* old_name, const char* new_name) {
    if (!folder) {
        printf("Error: Folder is NULL.\n");
        return 0;
    }

    FSFile* curr = folder->files_head;
    while (curr && strcmp(curr->name, old_name) != 0) curr = curr->next;
    if (!curr) {
        printf("Error: File '%s' not found in folder '%s'.\n", old_name, folder->name);
        return 0;
    }

    strncpy(curr->name, new_name, 99);
    curr->name[99] = '\0';
    curr->metadata.modified_time = time(NULL);
    snprintf(curr->relative_path, 500, "%s/%s", folder->relative_path, new_name);

    printf("File '%s' renamed to '%s' successfully.\n", old_name, new_name);
    return 1;
}

// Move file to another folder
int file_move(FSFile* file, FOLDER* new_parent) {
    if (!file || !new_parent) {
        printf("Error: Invalid file or target folder.\n");
        return 0;
    }

    FOLDER* old_parent = file->parent_folder;

    // Remove from old folder
    if (file->prev) file->prev->next = file->next;
    else old_parent->files_head = file->next;
    if (file->next) file->next->prev = file->prev;
    else old_parent->files_tail = file->prev;
    old_parent->file_count--;

    // Add to new folder
    file->parent_folder = new_parent;
    file->prev = new_parent->files_tail;
    file->next = NULL;
    if (new_parent->files_tail) new_parent->files_tail->next = file;
    else new_parent->files_head = file;
    new_parent->files_tail = file;
    new_parent->file_count++;

    snprintf(file->relative_path, 500, "%s/%s", new_parent->relative_path, file->name);
    printf("File '%s' moved to folder '%s' successfully.\n", file->name, new_parent->name);
    return 1;
}

// Show file info
void file_info(FSFile* file) {
    if (!file) {
        printf("Error: File is NULL.\n");
        return;
    }
    printf("File Info:\n");
    printf(" Name: %s\n", file->name);
    printf(" Size: %zu bytes\n", file->metadata.size);
    printf(" Created: %s", ctime(&file->metadata.created_time));
    printf(" Modified: %s", ctime(&file->metadata.modified_time));
    printf(" Path: %s\n", file->relative_path);
}

// List files in folder
void file_list(FOLDER* folder) {
    folder_list_files(folder);
}
