#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structure.h"
#include "filesystem.h"
#include "dir_ops.h"
#include "file_ops.h"
#include "tokenizer.h"
#include "resolve.h"

// Helper function to trim newline characters
static void trim_newline(char *str) {
    if (!str) return;
    size_t len = strlen(str);
    while (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r')) {
        str[--len] = '\0';
    }
}

// Display the menu
void display_menu(void) {
    printf("\nFILE SYSTEM SIMULATOR\n");
    printf("Current Directory: %s\n", get_current_path());
    printf("  DIRECTORY OPERATIONS:\n");
    printf("   1.  Initialize File System\n");
    printf("   2.  Create Directory (mkdir)\n");
    printf("   3.  Change Directory (cd)\n");
    printf("   4.  List Directory (ls)\n");
    printf("   5.  Print Working Directory (pwd)\n");
    printf("   6.  Remove Directory (rmdir)\n");
    printf("   7.  Rename Directory\n");
    printf("   8.  Move Directory\n");
    printf("   9.  Directory Info\n");
    printf("\n  FILE OPERATIONS:\n");
    printf("  10.  Create File (touch)\n");
    printf("  11.  Delete File (rm)\n");
    printf("  12.  Rename File\n");
    printf("  13.  Move File\n");
    printf("  14.  File Info\n");
    printf("  15.  List Files in Directory\n");
    printf("\n  NAVIGATION:\n");
    printf("  16.  Find Directory\n");
    printf("  17.  Print Directory Tree\n");
    printf("\n   0.  Exit\n");
    printf("Enter choice: ");
}

// Main function
int main(void) {
    char input[512];
    char path[256];
    char name[100];
    char new_name[100];
    int choice;
    int initialized = 0;

    printf("Welcome to File System Simulator!\n");
    printf("\nTip: Initialize the file system first (option 1)\n");

    while (1) {
        display_menu();
        
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        
        choice = atoi(input);

        // Check if filesystem is initialized (except for init and exit)
        if (!initialized && choice != 1 && choice != 0) {
            printf("\nError: File system not initialized! Please select option 1 first.\n");
            continue;
        }

        switch (choice) {
            case 1: // Initialize File System
                if (initialized) {
                    printf("\nWarning: File system already initialized!\n");
                    printf("Do you want to reinitialize? (y/n): ");
                    if (fgets(input, sizeof(input), stdin)) {
                        if (input[0] != 'y' && input[0] != 'Y') {
                            break;
                        }
                    }
                }
                init_filesystem();
                initialized = 1;
                printf("\nFile system initialized successfully!\n");
                printf("  Root directory created: /\n");
                printf("  Current directory: /\n");
                break;

            case 2: // Create Directory (mkdir)
                printf("\nEnter parent directory path (press Enter for current): ");
                if (!fgets(path, sizeof(path), stdin)) break;
                trim_newline(path);
                
                printf("Enter new directory name: ");
                if (!fgets(name, sizeof(name), stdin)) break;
                trim_newline(name);
                
                if (strlen(name) == 0) {
                    printf("Error: Directory name cannot be empty!\n");
                    break;
                }
                
                {
                    FOLDER *parent = (strlen(path) == 0) ? current_folder : resolve_dir(path);
                    if (parent) {
                        FOLDER *new_dir = dir_new(name, parent);
                        if (new_dir) {
                            dir_add_child(parent, new_dir);
                            printf("Directory '%s' created successfully at %s\n", 
                                   name, parent->relative_path);
                        } else {
                            printf("Error: Failed to create directory!\n");
                        }
                    } else {
                        printf("Error: Parent directory not found!\n");
                    }
                }
                break;

            case 3: // Change Directory (cd)
                printf("\nEnter directory path: ");
                if (!fgets(path, sizeof(path), stdin)) break;
                trim_newline(path);
                
                if (strlen(path) == 0) {
                    printf("Error: Path cannot be empty!\n");
                    break;
                }
                
                change_directory(path);
                break;

            case 4: // List Directory (ls)
                printf("\nEnter directory path (press Enter for current): ");
                if (!fgets(path, sizeof(path), stdin)) break;
                trim_newline(path);
                
                {
                    FOLDER *dir = (strlen(path) == 0) ? current_folder : resolve_dir(path);
                    if (dir) {
                        printf("\nContents of %s:\n", dir->relative_path);
                        print_directory(dir);
                    } else {
                        printf("Error: Directory not found!\n");
                    }
                }
                break;

            case 5: // Print Working Directory (pwd)
                printf("\nCurrent Directory: %s\n", get_current_path());
                break;

            case 6: // Remove Directory (rmdir)
                printf("\nEnter directory name to remove: ");
                if (!fgets(name, sizeof(name), stdin)) break;
                trim_newline(name);
                
                if (strlen(name) == 0) {
                    printf("Error: Directory name cannot be empty!\n");
                    break;
                }
                
                printf("Warning: This will remove the directory '%s'\n", name);
                printf("Are you sure? (y/n): ");
                if (fgets(input, sizeof(input), stdin)) {
                    if (input[0] == 'y' || input[0] == 'Y') {
                        rmdir_dir(root_folder, name);
                    } else {
                        printf("Operation cancelled.\n");
                    }
                }
                break;

            case 7: // Rename Directory
                printf("\nEnter directory path to rename: ");
                if (!fgets(path, sizeof(path), stdin)) break;
                trim_newline(path);
                
                printf("Enter new name: ");
                if (!fgets(new_name, sizeof(new_name), stdin)) break;
                trim_newline(new_name);
                
                if (strlen(path) == 0 || strlen(new_name) == 0) {
                    printf("Error: Path and name cannot be empty!\n");
                    break;
                }
                
                {
                    FOLDER *dir = resolve_dir(path);
                    if (dir) {
                        rename_folder(root_folder, dir->name, new_name);
                    } else {
                        printf("Error: Directory not found!\n");
                    }
                }
                break;

            case 8: // Move Directory
                printf("\nEnter source directory name: ");
                if (!fgets(name, sizeof(name), stdin)) break;
                trim_newline(name);
                
                printf("Enter destination directory path: ");
                if (!fgets(path, sizeof(path), stdin)) break;
                trim_newline(path);
                
                if (strlen(name) == 0 || strlen(path) == 0) {
                    printf("Error: Source and destination cannot be empty!\n");
                    break;
                }
                
                move_folder(root_folder, name, path);
                break;

            case 9: // Directory Info
                printf("\nEnter directory path (press Enter for current): ");
                if (!fgets(path, sizeof(path), stdin)) break;
                trim_newline(path);
                
                {
                    FOLDER *dir = (strlen(path) == 0) ? current_folder : resolve_dir(path);
                    if (dir) {
                        printf("\n");
                        print_dir_info(dir);
                    } else {
                        printf("Error: Directory not found!\n");
                    }
                }
                break;

            case 10: // Create File (touch)
                printf("\nEnter parent directory path (press Enter for current): ");
                if (!fgets(path, sizeof(path), stdin)) break;
                trim_newline(path);
                
                printf("Enter file name: ");
                if (!fgets(name, sizeof(name), stdin)) break;
                trim_newline(name);
                
                printf("Enter file size (bytes): ");
                if (!fgets(input, sizeof(input), stdin)) break;
                size_t size = atoi(input);
                
                if (strlen(name) == 0) {
                    printf("Error: File name cannot be empty!\n");
                    break;
                }
                
                {
                    FOLDER *parent = (strlen(path) == 0) ? current_folder : resolve_dir(path);
                    if (parent) {
                        FSFile *file = file_new(name, parent, size);
                        if (file) {
                            printf("File '%s' created successfully (size: %zu bytes)\n", 
                                   name, size);
                        } else {
                            printf("Error: Failed to create file!\n");
                        }
                    } else {
                        printf("Error: Parent directory not found!\n");
                    }
                }
                break;

            case 11: // Delete File (rm)
                printf("\nEnter file path: ");
                if (!fgets(path, sizeof(path), stdin)) break;
                trim_newline(path);
                
                if (strlen(path) == 0) {
                    printf("Error: File path cannot be empty!\n");
                    break;
                }
                
                {
                    FSFile *file = resolve_file(path);
                    if (file) {
                        printf("Warning: This will delete '%s'\n", file->name);
                        printf("Are you sure? (y/n): ");
                        if (fgets(input, sizeof(input), stdin)) {
                            if (input[0] == 'y' || input[0] == 'Y') {
                                file_delete(file->parent_folder, file->name);
                            } else {
                                printf("Operation cancelled.\n");
                            }
                        }
                    } else {
                        printf("Error: File not found!\n");
                    }
                }
                break;

            case 12: // Rename File
                printf("\nEnter file path: ");
                if (!fgets(path, sizeof(path), stdin)) break;
                trim_newline(path);
                
                printf("Enter new name: ");
                if (!fgets(new_name, sizeof(new_name), stdin)) break;
                trim_newline(new_name);
                
                if (strlen(path) == 0 || strlen(new_name) == 0) {
                    printf("Error: Path and name cannot be empty!\n");
                    break;
                }
                
                {
                    FSFile *file = resolve_file(path);
                    if (file) {
                        file_rename(file->parent_folder, file->name, new_name);
                    } else {
                        printf("Error: File not found!\n");
                    }
                }
                break;

            case 13: // Move File
                printf("\nEnter source file path: ");
                if (!fgets(path, sizeof(path), stdin)) break;
                trim_newline(path);
                
                printf("Enter destination directory path: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                trim_newline(input);
                
                if (strlen(path) == 0 || strlen(input) == 0) {
                    printf("Error: Source and destination cannot be empty!\n");
                    break;
                }
                
                {
                    FSFile *file = resolve_file(path);
                    FOLDER *dest = resolve_dir(input);
                    if (file && dest) {
                        file_move(file, dest);
                    } else {
                        if (!file) printf("Error: Source file not found!\n");
                        if (!dest) printf("Error: Destination directory not found!\n");
                    }
                }
                break;

            case 14: // File Info
                printf("\nEnter file path: ");
                if (!fgets(path, sizeof(path), stdin)) break;
                trim_newline(path);
                
                if (strlen(path) == 0) {
                    printf("Error: File path cannot be empty!\n");
                    break;
                }
                
                {
                    FSFile *file = resolve_file(path);
                    if (file) {
                        printf("\n");
                        file_info(file);
                    } else {
                        printf("Error: File not found!\n");
                    }
                }
                break;

            case 15: // List Files in Directory
                printf("\nEnter directory path (press Enter for current): ");
                if (!fgets(path, sizeof(path), stdin)) break;
                trim_newline(path);
                
                {
                    FOLDER *dir = (strlen(path) == 0) ? current_folder : resolve_dir(path);
                    if (dir) {
                        printf("\nFiles in %s:\n", dir->relative_path);
                        print(dir,0);
                    } else {
                        printf("Error: Directory not found!\n");
                    }
                }
                break;

            case 16: // Find Directory
                printf("\nEnter directory name to find: ");
                if (!fgets(name, sizeof(name), stdin)) break;
                trim_newline(name);
                
                if (strlen(name) == 0) {
                    printf("Error: Directory name cannot be empty!\n");
                    break;
                }
                
                {
                    FOLDER *found = find_dir(root_folder, name);
                    if (found) {
                        printf("Found directory: %s\n", found->relative_path);
                    } else {
                        printf("Directory '%s' not found in the file system.\n", name);
                    }
                }
                break;

            case 17: // Print Directory Tree
                printf("\nEnter directory path (press Enter for root): ");
                if (!fgets(path, sizeof(path), stdin)) break;
                trim_newline(path);
                
                {
                    FOLDER *dir = (strlen(path) == 0) ? root_folder : resolve_dir(path);
                    if (dir) {
                        printf("\nDirectory Tree from %s:\n", dir->relative_path);
                        print(dir, 0);
                    } else {
                        printf("Error: Directory not found!\n");
                    }
                }
                break;

            case 0: // Exit
                printf("Thank you for using File System Simulator!\n");
                return 0;

            default:
                printf("\nInvalid choice! Please select a valid option (0-17).\n");
        }
    }

    return 0;
}