#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>
#include "dir_ops.h"
#include "structure.h"


FOLDER* dir_new(char *name, FOLDER *parent) {
    FOLDER *new_dir = (FOLDER*)malloc(sizeof(FOLDER));
    if (!new_dir) {
        printf("Memory allocation failed for directory '%s'\n", name);
        return NULL;
    }

    strcpy(new_dir->name, name);
    new_dir->first_child = NULL;
    new_dir->last_child = NULL;
    new_dir->next_sibling = NULL;
    new_dir->prev_sibling = NULL;
    new_dir->files_head = NULL;
    new_dir->files_tail = NULL;
    new_dir->parent = parent;
    
    // Initialize counts
    new_dir->file_count = 0;
    new_dir->folder_count = 0;

    // Initialize metadata
    new_dir->metadata.created_time = time(NULL);
    new_dir->metadata.modified_time = new_dir->metadata.created_time;
    new_dir->metadata.size = 0;

    // Generate relative path
    if (parent)
        snprintf(new_dir->relative_path, sizeof(new_dir->relative_path), "%s/%s", parent->relative_path, name);
    else
        snprintf(new_dir->relative_path, sizeof(new_dir->relative_path), "%s", name);

    return new_dir;
}

void dir_add_child(FOLDER *parent, FOLDER *child) {
    if (!parent || !child) return;

    if (!parent->first_child)
        parent->first_child = child;
    else {
        parent->last_child->next_sibling = child;
        child->prev_sibling = parent->last_child;
    }

    parent->last_child = child;
    child->parent = parent;
    
    // Increment folder count
    parent->folder_count++;
    
    // Update parent's modified time
    parent->metadata.modified_time = time(NULL);
}



FOLDER* find_dir(FOLDER *root, char *name) {
    if (!root) return NULL;
    if (strcmp(root->name, name) == 0) return root;

    FOLDER *child = root->first_child;
    while (child) {
        FOLDER *found = find_dir(child, name);
        if (found) return found;
        child = child->next_sibling;
    }
    return NULL;
}





void mkdir_dir(FOLDER *root, char *path) {
    if (!root || !path) return;

    FOLDER *parent = find_dir(root, path);
    if (!parent) {
        printf("Parent directory '%s' not found.\n", path);
        return;
    }

    char new_name[100];
    printf("Enter new folder name under '%s': ", path);
    scanf("%s", new_name);

    FOLDER *new_dir = dir_new(new_name, parent);
    dir_add_child(parent, new_dir);
    printf("Directory '%s' created successfully under '%s'\n", new_name, path);
}





void move_folder(FOLDER *root, char *src_name, char *dest_name) {
    FOLDER *source = find_dir(root, src_name);
    FOLDER *destination = find_dir(root, dest_name);

    if (!source || !destination) {
        printf("Source or destination not found.\n");
        return;
    }

    FOLDER *temp = destination;
    while (temp) {
        if (temp == source) {
            printf("Cannot move folder inside itself or subfolder.\n");
            return;
        }
        temp = temp->parent;
    }

    FOLDER *parent = source->parent;
    if (!parent) {
        printf("Cannot move root folder.\n");
        return;
    }

    if (parent->first_child == source)
        parent->first_child = source->next_sibling;
    if (parent->last_child == source)
        parent->last_child = source->prev_sibling;
    if (source->prev_sibling)
        source->prev_sibling->next_sibling = source->next_sibling;
    if (source->next_sibling)
        source->next_sibling->prev_sibling = source->prev_sibling;

    parent->folder_count--;
    source->next_sibling = source->prev_sibling = NULL;

    dir_add_child(destination, source);
    source->parent = destination;

    snprintf(source->relative_path, sizeof(source->relative_path),
             "%s/%s", destination->relative_path, source->name);

    printf("Folder '%s' moved to '%s'\n", src_name, dest_name);
}






void rename_folder(FOLDER *root, char *old_name, char *new_name) {
    FOLDER *target = find_dir(root, old_name);
    if (!target) {
        printf("Folder '%s' not found.\n", old_name);
        return;
    }

    // Check if target is root folder
    if (!target->parent) {
        printf("Cannot rename root folder.\n");
        return;
    }

    // Check if new name already exists in the same parent directory
    FOLDER *sibling = target->parent->first_child;
    while (sibling) {
        if (sibling != target && strcmp(sibling->name, new_name) == 0) {
            printf("Error: Folder '%s' already exists in '%s'.\n", 
                   new_name, target->parent->name);
            return;
        }
        sibling = sibling->next_sibling;
    }

    // Update the name
    strcpy(target->name, new_name);
    
    // Update the relative path
    snprintf(target->relative_path, sizeof(target->relative_path),
             "%s/%s", target->parent->relative_path, new_name);

    // Update modified time of parent
    target->parent->metadata.modified_time = time(NULL);

    printf("Folder '%s' renamed to '%s'\n", old_name, new_name);
}




void rmdir_dir(FOLDER *root, char *name) {
    FOLDER *target = find_dir(root, name);
    if (!target || !target->parent) {
        printf("Cannot remove root or '%s' not found.\n", name);
        return;
    }

    // Check if directory is empty
    if (target->folder_count > 0) {
        printf("Error: Cannot remove '%s' - directory contains %d subfolder(s).\n", 
               name, target->folder_count);
        return;
    }
    
    if (target->file_count > 0) {
        printf("Error: Cannot remove '%s' - directory contains %d file(s).\n", 
               name, target->file_count);
        return;
    }

    FOLDER *parent = target->parent;

    // Remove from parent's linked list
    if (parent->first_child == target)
        parent->first_child = target->next_sibling;
    if (parent->last_child == target)
        parent->last_child = target->prev_sibling;
    if (target->prev_sibling)
        target->prev_sibling->next_sibling = target->next_sibling;
    if (target->next_sibling)
        target->next_sibling->prev_sibling = target->prev_sibling;

    // Update parent's folder count
    parent->folder_count--;
    
    // Update parent's modified time
    parent->metadata.modified_time = time(NULL);

    // Free the directory
    free(target);

    printf("Directory '%s' removed successfully.\n", name);
}

// // Optional: Recursive remove function
// void rmdir_recursive(FOLDER *root, char *name) {
//     FOLDER *target = find_dir(root, name);
//     if (!target || !target->parent) {
//         printf("Cannot remove root or '%s' not found.\n", name);
//         return;
//     }

//     // First remove all files (you'll need to implement this based on your file operations)
//     while (target->files_head) {
//         // Remove files - assuming you have a file_delete function
//         // file_delete(target, target->files_head->name);
//     }

//     // Recursively remove all subdirectories
//     while (target->first_child) {
//         rmdir_recursive(target, target->first_child->name);
//     }

//     // Now remove the empty directory
//     rmdir_dir(root, name);
// }







void print(FOLDER *dir, int level) {
    if (!dir) return;

    // Indentation
    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    // Format creation time
    char time_str[64];
    struct tm *tm_info = localtime(&dir->metadata.created_time);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

    // Print directory metadata
    printf("|- %s  [Created: %s | Size: %zu bytes | Files: %d | Folders: %d]\n", 
           dir->name, time_str, dir->metadata.size, dir->file_count, dir->folder_count);

    // Print files inside this directory
    FSFile *file = dir->files_head;
    while (file) {
        for (int i = 0; i < level + 1; i++) {
            printf("  ");
        }
        
        // Format file creation time
        struct tm *file_tm_info = localtime(&file->metadata.created_time);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", file_tm_info);
        
        printf("|- %s  [Created: %s | Size: %zu bytes]\n", 
               file->name, time_str, file->metadata.size);
        file = file->next;
    }

    // Recurse into subdirectories
    FOLDER *child = dir->first_child;
    while (child) {
        print(child, level + 1);
        child = child->next_sibling;
    }
}

void print_directory(FOLDER *root) {
    if (!root) {
        printf("No directories to display.\n");
        return;
    }

    printf("\nDirectory Structure:\n");
    print(root, 0);
}

// Additional utility function to print directory path
void print_path(FOLDER *dir) {
    if (!dir) return;
    printf("Current path: %s\n", dir->relative_path);
}

// Function to print directory info only (without recursion)
void print_dir_info(FOLDER *dir) {
    if (!dir) {
        printf("Directory not found.\n");
        return;
    }

    char created_str[64], modified_str[64];
    struct tm *tm_info;
    
    tm_info = localtime(&dir->metadata.created_time);
    strftime(created_str, sizeof(created_str), "%Y-%m-%d %H:%M:%S", tm_info);
    
    tm_info = localtime(&dir->metadata.modified_time);
    strftime(modified_str, sizeof(modified_str), "%Y-%m-%d %H:%M:%S", tm_info);

    printf("\nDirectory Information:\n");
    printf("Name: %s\n", dir->name);
    printf("Path: %s\n", dir->relative_path);
    printf("Created: %s\n", created_str);
    printf("Modified: %s\n", modified_str);
    printf("Size: %zu bytes\n", dir->metadata.size);
    printf("Files: %d\n", dir->file_count);
    printf("Folders: %d\n", dir->folder_count);
}   