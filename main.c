#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structure.h"
#include "filesystem.h"
#include "dir_ops.h"
#include "file_ops.h"
#include "tokenizer.h"
#include "resolve.h"

#include "storagefolder.h"
#include "file_content.h"

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
    printf("\n  STORAGE:\n");
    printf("  18.  Save Filesystem\n");
    printf("  19.  Load Filesystem\n");
    printf("  20.  Write Text to File\n");
    printf("  21.  Read Text from File\n");

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
        // Try to auto-load existing filesystem

    printf("Checking for saved filesystem...\n");
    FOLDER *loaded = load_filesystem();
    if (loaded) {
        root_folder = loaded;
        current_folder = root_folder;
        initialized = 1;
        load_file_contents();  // IMPORTANT: Also load text content!
        printf("Filesystem and content loaded successfully.\n");
    } else {
        printf("\nTip: Initialize the file system first (option 1)\n");
    }
    

    // printf("\nTip: Initialize the file system first (option 1)\n");
    display_menu();
    while (1) {
        
        
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
                load_file_contents();
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

            case 18: // Save Filesystem
                save_filesystem(root_folder);
                save_file_contents();  
                printf("Filesystem saved.\n");
                break;

            case 19: // Load Filesystem
                {
                    FOLDER *loaded = load_filesystem();
                    if (loaded) {
                        root_folder = loaded;
                        current_folder = root_folder;
                        initialized = 1;
                        load_file_contents();
                        printf("Filesystem loaded.\n");
                    } else {
                        printf("No saved filesystem found.\n");
                    }
                }
                break;

            case 20: // Write Text to File
                printf("\nEnter file name in current directory: ");
                if (!fgets(name, sizeof(name), stdin)) break;
                trim_newline(name);
                
                {
                    FSFile *file = NULL;
                    FSFile *temp = current_folder->files_head;
                    while (temp) {
                        if (strcmp(temp->name, name) == 0) {
                            file = temp;
                            break;
                        }
                        temp = temp->next;
                    }
                    
                    if (file) {
                        write_text_to_file(file);
                    } else {
                        printf("File not found in current directory.\n");
                    }
                }
                break;

            case 21: // Read Text from File
                printf("\nEnter file name in current directory: ");
                if (!fgets(name, sizeof(name), stdin)) break;
                trim_newline(name);
                
                {
                    FSFile *file = NULL;
                    FSFile *temp = current_folder->files_head;
                    while (temp) {
                        if (strcmp(temp->name, name) == 0) {
                            file = temp;
                            break;
                        }
                        temp = temp->next;
                    }
                    
                    if (file) {
                        read_text_from_file(file);
                    } else {
                        printf("File not found in current directory.\n");
                    }
                }
                break;




            case 0: // Exit
                printf("Saving filesystem...\n");
                save_filesystem(root_folder);
                save_file_contents();  // ADD THIS
                printf("Thank you for using File System Simulator!\n");
                return 0;

            default:
                printf("\nInvalid choice! Please select a valid option (0-21).\n");
        }
    }

    return 0;
}























// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <signal.h>
// #include "structure.h"
// #include "filesystem.h"
// #include "dir_ops.h"
// #include "file_ops.h"
// #include "tokenizer.h"
// #include "resolve.h"
// #include "storagefolder.h"
// #include "file_content.h"

// // Global flag for auto-save
// static int auto_save_enabled = 1;
// static const char* SAVE_FILE = "filesystem.dat";

// // Auto-save wrapper function
// void auto_save() {
//     if (auto_save_enabled && root_folder) {
//         save_filesystem(root_folder, SAVE_FILE);
//         printf("[Auto-saved]\n");
//     }
// }

// // Signal handler for clean exit
// void handle_exit_signal(int sig) {
//     printf("\nSaving filesystem before exit...\n");
//     if (root_folder) {
//         save_filesystem(root_folder, SAVE_FILE);
//     }
//     exit(0);
// }

// // Helper function to trim newline characters
// static void trim_newline(char *str) {
//     if (!str) return;
//     size_t len = strlen(str);
//     while (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r')) {
//         str[--len] = '\0';
//     }
// }

// // Display the menu
// void display_menu(void) {
//     printf("\nFILE SYSTEM SIMULATOR\n");
//     printf("Current Directory: %s\n", get_current_path());
//     printf("Auto-save: %s\n", auto_save_enabled ? "ON" : "OFF");
//     printf("  DIRECTORY OPERATIONS:\n");
//     printf("   1.  Initialize/Load File System\n");
//     printf("   2.  Create Directory (mkdir)\n");
//     printf("   3.  Change Directory (cd)\n");
//     printf("   4.  List Directory (ls)\n");
//     printf("   5.  Print Working Directory (pwd)\n");
//     printf("   6.  Remove Directory (rmdir)\n");
//     printf("   7.  Rename Directory\n");
//     printf("   8.  Move Directory\n");
//     printf("   9.  Directory Info\n");
//     printf("\n  FILE OPERATIONS:\n");
//     printf("  10.  Create File (touch)\n");
//     printf("  11.  Delete File (rm)\n");
//     printf("  12.  Rename File\n");
//     printf("  13.  Move File\n");
//     printf("  14.  File Info\n");
//     printf("  15.  List Files in Directory\n");
//     printf("\n  NAVIGATION:\n");
//     printf("  16.  Find Directory\n");
//     printf("  17.  Print Directory Tree\n");
//     printf("\n  STORAGE:\n");
//     printf("  18.  Manual Save\n");
//     printf("  19.  Reload from Disk\n");
//     printf("  20.  Toggle Auto-save\n");
//     printf("  21.  Clear and Start Fresh\n");
//     printf("\n  FILE CONTENT OPERATIONS:\n");
//     printf("  22.  Write Content to File\n");
//     printf("  23.  Edit File Content\n");
//     printf("  24.  View File Content\n");
//     printf("  25.  Export File to Disk\n");
//     printf("  26.  Import File from Disk\n");
//     printf("  27.  Clear File Content\n");
//     printf("  28.  Content Statistics\n");

//     printf("\n   0.  Exit (with save)\n");
//     printf("Enter choice: ");
// }

// // Initialize or load filesystem
// int init_or_load_filesystem() {
//     // First, try to load existing filesystem
//     FOLDER* loaded = load_filesystem(SAVE_FILE);
    
//     if (loaded) {
//         root_folder = loaded;
//         current_folder = root_folder;
//         printf("Filesystem loaded from '%s'.\n", SAVE_FILE);
//         printf("Restored %d folders and files.\n", count_all_items(root_folder));
//         return 1;
//     } else {
//         // No saved filesystem, create new one
//         printf("No saved filesystem found. Creating new filesystem...\n");
//         init_filesystem();
//         root_folder = get_root();
//         current_folder = root_folder;
//         printf("New filesystem initialized.\n");
//         auto_save();
//         return 1;
//     }
// }

// // Count all items in filesystem (helper function)
// int count_all_items(FOLDER* folder) {
//     if (!folder) return 0;
    
//     int count = 1; // Count this folder
//     count += folder->file_count;
    
//     FOLDER* child = folder->first_child;
//     while (child) {
//         count += count_all_items(child);
//         child = child->next_sibling;
//     }
//     return count;
// }

// // Main function with persistence
// int main(void) {
//     char input[512];
//     char path[256];
//     char name[100];
//     char new_name[100];
//     int choice;
//     int initialized = 0;

//     // Set up signal handlers for clean exit
//     signal(SIGINT, handle_exit_signal);
//     signal(SIGTERM, handle_exit_signal);

//     printf("Welcome to File System Simulator!\n");
//     printf("=================================\n");
    
//     // Try to auto-load on startup
//     printf("\nChecking for saved filesystem...\n");
//     initialized = init_or_load_filesystem();
    
//     while (1) {
//         display_menu();
        
//         if (!fgets(input, sizeof(input), stdin)) {
//             break;
//         }
        
//         choice = atoi(input);

//         // Check if filesystem is initialized (except for init, exit, and storage operations)
//         if (!initialized && choice != 1 && choice != 0 && choice != 18 && choice != 19 && choice != 21) {
//             printf("\nError: File system not initialized! Please select option 1 first.\n");
//             continue;
//         }

//         switch (choice) {
//             case 1: // Initialize/Load File System
//                 if (initialized) {
//                     printf("\nFile system already loaded!\n");
//                     printf("Choose an option:\n");
//                     printf("1. Reload from disk (lose unsaved changes)\n");
//                     printf("2. Clear and start fresh\n");
//                     printf("3. Cancel\n");
//                     printf("Choice: ");
                    
//                     if (fgets(input, sizeof(input), stdin)) {
//                         int sub_choice = atoi(input);
//                         if (sub_choice == 1) {
//                             FOLDER* loaded = load_filesystem(SAVE_FILE);
//                             if (loaded) {
//                                 root_folder = loaded;
//                                 current_folder = root_folder;
//                                 printf("Filesystem reloaded from disk.\n");
//                             }
//                         } else if (sub_choice == 2) {
//                             init_filesystem();
//                             printf("New filesystem initialized.\n");
//                             auto_save();
//                         }
//                     }
//                 } else {
//                     initialized = init_or_load_filesystem();
//                 }
//                 break;

//             case 2: // Create Directory (mkdir)
//                 printf("\nEnter parent directory path (press Enter for current): ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 printf("Enter new directory name: ");
//                 if (!fgets(name, sizeof(name), stdin)) break;
//                 trim_newline(name);
                
//                 if (strlen(name) == 0) {
//                     printf("Error: Directory name cannot be empty!\n");
//                     break;
//                 }
                
//                 {
//                     FOLDER *parent = (strlen(path) == 0) ? current_folder : resolve_dir(path);
//                     if (parent) {
//                         FOLDER *new_dir = dir_new(name, parent);
//                         if (new_dir) {
//                             dir_add_child(parent, new_dir);
//                             printf("Directory '%s' created successfully at %s\n", 
//                                    name, parent->relative_path);
//                             auto_save(); // Auto-save after creating directory
//                         } else {
//                             printf("Error: Failed to create directory!\n");
//                         }
//                     } else {
//                         printf("Error: Parent directory not found!\n");
//                     }
//                 }
//                 break;

//             case 3: // Change Directory (cd)
//                 printf("\nEnter directory path: ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 if (strlen(path) == 0) {
//                     printf("Error: Path cannot be empty!\n");
//                     break;
//                 }
                
//                 change_directory(path);
//                 break;

//             case 4: // List Directory (ls)
//                 printf("\nEnter directory path (press Enter for current): ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 {
//                     FOLDER *dir = (strlen(path) == 0) ? current_folder : resolve_dir(path);
//                     if (dir) {
//                         printf("\nContents of %s:\n", dir->relative_path);
//                         print_directory(dir);
//                     } else {
//                         printf("Error: Directory not found!\n");
//                     }
//                 }
//                 break;

//             case 5: // Print Working Directory (pwd)
//                 printf("\nCurrent Directory: %s\n", get_current_path());
//                 break;

//             case 6: // Remove Directory (rmdir)
//                 printf("\nEnter directory name to remove: ");
//                 if (!fgets(name, sizeof(name), stdin)) break;
//                 trim_newline(name);
                
//                 if (strlen(name) == 0) {
//                     printf("Error: Directory name cannot be empty!\n");
//                     break;
//                 }
                
//                 printf("Warning: This will remove the directory '%s'\n", name);
//                 printf("Are you sure? (y/n): ");
//                 if (fgets(input, sizeof(input), stdin)) {
//                     if (input[0] == 'y' || input[0] == 'Y') {
//                         if (rmdir_dir(root_folder, name)) {
//                             auto_save(); // Auto-save after removing directory
//                         }
//                     } else {
//                         printf("Operation cancelled.\n");
//                     }
//                 }
//                 break;

//             case 7: // Rename Directory
//                 printf("\nEnter directory path to rename: ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 printf("Enter new name: ");
//                 if (!fgets(new_name, sizeof(new_name), stdin)) break;
//                 trim_newline(new_name);
                
//                 if (strlen(path) == 0 || strlen(new_name) == 0) {
//                     printf("Error: Path and name cannot be empty!\n");
//                     break;
//                 }
                
//                 {
//                     FOLDER *dir = resolve_dir(path);
//                     if (dir) {
//                         rename_folder(root_folder, dir->name, new_name);
//                         auto_save(); // Auto-save after renaming
//                     } else {
//                         printf("Error: Directory not found!\n");
//                     }
//                 }
//                 break;

//             case 8: // Move Directory
//                 printf("\nEnter source directory name: ");
//                 if (!fgets(name, sizeof(name), stdin)) break;
//                 trim_newline(name);
                
//                 printf("Enter destination directory path: ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 if (strlen(name) == 0 || strlen(path) == 0) {
//                     printf("Error: Source and destination cannot be empty!\n");
//                     break;
//                 }
                
//                 move_folder(root_folder, name, path);
//                 auto_save(); // Auto-save after moving
//                 break;

//             case 10: // Create File (touch)
//                 printf("\nEnter parent directory path (press Enter for current): ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 printf("Enter file name: ");
//                 if (!fgets(name, sizeof(name), stdin)) break;
//                 trim_newline(name);
                
//                 printf("Enter file size (bytes): ");
//                 if (!fgets(input, sizeof(input), stdin)) break;
//                 size_t size = atoi(input);
                
//                 if (strlen(name) == 0) {
//                     printf("Error: File name cannot be empty!\n");
//                     break;
//                 }
                
//                 {
//                     FOLDER *parent = (strlen(path) == 0) ? current_folder : resolve_dir(path);
//                     if (parent) {
//                         FSFile *file = file_new(name, parent, size);
//                         if (file) {
//                             printf("File '%s' created successfully (size: %zu bytes)\n", 
//                                    name, size);
//                             auto_save(); // Auto-save after creating file
//                         } else {
//                             printf("Error: Failed to create file!\n");
//                         }
//                     } else {
//                         printf("Error: Parent directory not found!\n");
//                     }
//                 }
//                 break;

//             case 11: // Delete File (rm)
//                 printf("\nEnter file path: ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 if (strlen(path) == 0) {
//                     printf("Error: File path cannot be empty!\n");
//                     break;
//                 }
                
//                 {
//                     FSFile *file = resolve_file(path);
//                     if (file) {
//                         printf("Warning: This will delete '%s'\n", file->name);
//                         printf("Are you sure? (y/n): ");
//                         if (fgets(input, sizeof(input), stdin)) {
//                             if (input[0] == 'y' || input[0] == 'Y') {
//                                 if (file_delete(file->parent_folder, file->name)) {
//                                     auto_save(); // Auto-save after deleting file
//                                 }
//                             } else {
//                                 printf("Operation cancelled.\n");
//                             }
//                         }
//                     } else {
//                         printf("Error: File not found!\n");
//                     }
//                 }
//                 break;

//             case 12: // Rename File
//                 printf("\nEnter file path: ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 printf("Enter new name: ");
//                                 if (!fgets(new_name, sizeof(new_name), stdin)) break;
//                 trim_newline(new_name);
//                 if (strlen(path) == 0 || strlen(new_name) == 0) {
//                     printf("Error: Path and name cannot be empty!\n");
//                     break;
//                 }
                
//                 {
//                     FSFile *file = resolve_file(path);
//                     if (file) {
//                         if (file_rename(file->parent_folder, file->name, new_name)) {
//                             auto_save(); // Auto-save after renaming file
//                         }
//                     } else {
//                         printf("Error: File not found!\n");
//                     }
//                 }
//                 break;

//             case 13: // Move File
//                 printf("\nEnter source file path: ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 printf("Enter destination directory path: ");
//                 if (!fgets(input, sizeof(input), stdin)) break;
//                 trim_newline(input);
                
//                 if (strlen(path) == 0 || strlen(input) == 0) {
//                     printf("Error: Source and destination cannot be empty!\n");
//                     break;
//                 }
                
//                 {
//                     FSFile *file = resolve_file(path);
//                     FOLDER *dest = resolve_dir(input);
//                     if (file && dest) {
//                         if (file_move(file, dest)) {
//                             auto_save(); // Auto-save after moving file
//                         }
//                     } else {
//                         if (!file) printf("Error: Source file not found!\n");
//                         if (!dest) printf("Error: Destination directory not found!\n");
//                     }
//                 }
//                 break;

//             case 14: // File Info
//                 printf("\nEnter file path: ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 if (strlen(path) == 0) {
//                     printf("Error: File path cannot be empty!\n");
//                     break;
//                 }
                
//                 {
//                     FSFile *file = resolve_file(path);
//                     if (file) {
//                         printf("\n");
//                         file_info(file);
//                     } else {
//                         printf("Error: File not found!\n");
//                     }
//                 }
//                 break;

//             case 15: // List Files in Directory
//                 printf("\nEnter directory path (press Enter for current): ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 {
//                     FOLDER *dir = (strlen(path) == 0) ? current_folder : resolve_dir(path);
//                     if (dir) {
//                         printf("\nFiles in %s:\n", dir->relative_path);
//                         print(dir, 0);
//                     } else {
//                         printf("Error: Directory not found!\n");
//                     }
//                 }
//                 break;

//             case 16: // Find Directory
//                 printf("\nEnter directory name to find: ");
//                 if (!fgets(name, sizeof(name), stdin)) break;
//                 trim_newline(name);
                
//                 if (strlen(name) == 0) {
//                     printf("Error: Directory name cannot be empty!\n");
//                     break;
//                 }
                
//                 {
//                     FOLDER *found = find_dir(root_folder, name);
//                     if (found) {
//                         printf("Found directory: %s\n", found->relative_path);
//                         print_dir_info(found);
//                     } else {
//                         printf("Directory '%s' not found in the file system.\n", name);
//                     }
//                 }
//                 break;

//             case 17: // Print Directory Tree
//                 printf("\nEnter directory path (press Enter for root): ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 {
//                     FOLDER *dir = (strlen(path) == 0) ? root_folder : resolve_dir(path);
//                     if (dir) {
//                         printf("\nDirectory Tree from %s:\n", dir->relative_path);
//                         print(dir, 0);
//                     } else {
//                         printf("Error: Directory not found!\n");
//                     }
//                 }
//                 break;

//             case 18: // Manual Save
//                 printf("\nSaving filesystem to disk...\n");
//                 if (root_folder) {
//                     save_filesystem(root_folder, SAVE_FILE);
//                     printf("Filesystem saved successfully to '%s'.\n", SAVE_FILE);
                    
//                     // Show statistics
//                     int total_items = count_all_items(root_folder);
//                     printf("Saved %d total items (folders and files).\n", total_items);
//                 } else {
//                     printf("Error: No filesystem to save!\n");
//                 }
//                 break;

//             case 19: // Reload from Disk
//                 printf("\nWarning: This will discard any unsaved changes!\n");
//                 printf("Are you sure you want to reload from disk? (y/n): ");
//                 if (fgets(input, sizeof(input), stdin)) {
//                     if (input[0] == 'y' || input[0] == 'Y') {
//                         FOLDER* loaded = load_filesystem(SAVE_FILE);
//                         if (loaded) {
//                             // Free old filesystem if needed
//                             // (In production, you'd want a proper free_filesystem function)
//                             root_folder = loaded;
//                             current_folder = root_folder;
//                             printf("Filesystem reloaded from '%s'.\n", SAVE_FILE);
                            
//                             int total_items = count_all_items(root_folder);
//                             printf("Loaded %d total items.\n", total_items);
//                         } else {
//                             printf("Error: Failed to load filesystem from disk!\n");
//                         }
//                     } else {
//                         printf("Reload cancelled.\n");
//                     }
//                 }
//                 break;

//             case 20: // Toggle Auto-save
//                 auto_save_enabled = !auto_save_enabled;
//                 printf("\nAuto-save is now %s.\n", auto_save_enabled ? "ON" : "OFF");
//                 if (auto_save_enabled) {
//                     printf("Changes will be automatically saved after each operation.\n");
//                 } else {
//                     printf("Warning: Changes will NOT be saved automatically!\n");
//                     printf("Use option 18 to manually save your work.\n");
//                 }
//                 break;

//             case 21: // Clear and Start Fresh
//                 printf("\nWarning: This will delete the current filesystem and start fresh!\n");
//                 printf("Are you sure? (y/n): ");
//                 if (fgets(input, sizeof(input), stdin)) {
//                     if (input[0] == 'y' || input[0] == 'Y') {
//                         printf("Type 'DELETE' to confirm: ");
//                         if (fgets(input, sizeof(input), stdin)) {
//                             trim_newline(input);
//                             if (strcmp(input, "DELETE") == 0) {
//                                 // Free old filesystem (in production, use proper cleanup)
//                                 init_filesystem();
//                                 root_folder = get_root();
//                                 current_folder = root_folder;
//                                 printf("New filesystem initialized.\n");
                                
//                                 // Save the new empty filesystem
//                                 if (auto_save_enabled) {
//                                     save_filesystem(root_folder, SAVE_FILE);
//                                     printf("Empty filesystem saved.\n");
//                                 }
//                             } else {
//                                 printf("Confirmation failed. Operation cancelled.\n");
//                             }
//                         }
//                     } else {
//                         printf("Operation cancelled.\n");
//                     }
//                 }
//                 break;



//             case 22: // Write Content to File
//                 printf("\nEnter file path: ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 {
//                     FSFile *file = resolve_file(path);
//                     if (file) {
//                         if (write_file_content(file)) {
//                             printf("Content written successfully.\n");
//                             if (auto_save_enabled) {
//                                 save_filesystem_with_content(root_folder, SAVE_FILE);
//                                 printf("[Auto-saved with content]\n");
//                             }
//                         }
//                     } else {
//                         printf("Error: File not found!\n");
//                         printf("Tip: Create the file first using option 10.\n");
//                     }
//                 }
//                 break;

//             case 23: // Edit File Content
//                 printf("\nEnter file path: ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 {
//                     FSFile *file = resolve_file(path);
//                     if (file) {
//                         if (edit_file_content(file)) {
//                             printf("Content edited successfully.\n");
//                             if (auto_save_enabled) {
//                                 save_filesystem_with_content(root_folder, SAVE_FILE);
//                                 printf("[Auto-saved with content]\n");
//                             }
//                         }
//                     } else {
//                         printf("Error: File not found!\n");
//                     }
//                 }
//                 break;

//             case 24: // View File Content
//                 printf("\nEnter file path: ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 {
//                     FSFile *file = resolve_file(path);
//                     if (file) {
//                         view_file_content(file);
//                     } else {
//                         printf("Error: File not found!\n");
//                     }
//                 }
//                 break;

//             case 25: // Export File to Disk
//                 printf("\nEnter virtual file path: ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 printf("Enter output filename (press Enter for same name): ");
//                 if (!fgets(input, sizeof(input), stdin)) break;
//                 trim_newline(input);
                
//                 {
//                     FSFile *file = resolve_file(path);
//                     if (file) {
//                         const char* output = strlen(input) > 0 ? input : NULL;
//                         if (export_to_disk(file, output)) {
//                             printf("File exported successfully.\n");
//                         }
//                     } else {
//                         printf("Error: File not found in virtual filesystem!\n");
//                     }
//                 }
//                 break;

//             case 26: // Import File from Disk
//                 printf("\nEnter disk file to import: ");
//                 if (!fgets(input, sizeof(input), stdin)) break;
//                 trim_newline(input);
                
//                 printf("Enter virtual file path (where to store): ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 {
//                     FSFile *file = resolve_file(path);
//                     if (!file) {
//                         // File doesn't exist, try to create it
//                         char* last_slash = strrchr(path, '/');
//                         if (last_slash) {
//                             char dir_path[256];
//                             strncpy(dir_path, path, last_slash - path);
//                             dir_path[last_slash - path] = '\0';
                            
//                             FOLDER* parent = resolve_dir(dir_path);
//                             if (parent) {
//                                 // Use existing file_new from file_ops.c
//                                 file = file_new(last_slash + 1, parent, 0);
//                                 if (file) {
//                                     printf("Created new file: %s\n", file->name);
//                                 }
//                             } else {
//                                 printf("Error: Parent directory not found!\n");
//                                 break;
//                             }
//                         } else {
//                             // No path, create in current directory
//                             file = file_new(path, current_folder, 0);
//                         }
//                     }
                    
//                     if (file) {
//                         if (import_from_disk(file, input)) {
//                             printf("File imported successfully.\n");
//                             if (auto_save_enabled) {
//                                 save_filesystem_with_content(root_folder, SAVE_FILE);
//                                 printf("[Auto-saved with content]\n");
//                             }
//                         }
//                     }
//                 }
//                 break;

//             case 27: // Clear File Content
//                 printf("\nEnter file path: ");
//                 if (!fgets(path, sizeof(path), stdin)) break;
//                 trim_newline(path);
                
//                 {
//                     FSFile *file = resolve_file(path);
//                     if (file) {
//                         printf("Warning: This will delete all content in '%s'.\n", file->name);
//                         printf("Are you sure? (y/n): ");
//                         if (fgets(input, sizeof(input), stdin)) {
//                             if (input[0] == 'y' || input[0] == 'Y') {
//                                 clear_file_content(file);
//                                 printf("File content cleared.\n");
//                                 if (auto_save_enabled) {
//                                     save_filesystem_with_content(root_folder, SAVE_FILE);
//                                     printf("[Auto-saved]\n");
//                                 }
//                             } else {
//                                 printf("Operation cancelled.\n");
//                             }
//                         }
//                     } else {
//                         printf("Error: File not found!\n");
//                     }
//                 }
//                 break;

//             case 28: // Content Statistics
//                 show_content_stats();
//                 break;




//             case 0: // Exit with save
//                 printf("\nSaving filesystem before exit...\n");
//                 if (root_folder) {
//                     save_filesystem(root_folder, SAVE_FILE);
//                     int total_items = count_all_items(root_folder);
//                     printf("Saved %d items to '%s'.\n", total_items, SAVE_FILE);
//                 }
//                 printf("Thank you for using File System Simulator!\n");
//                 return 0;

//             default:
//                 printf("\nInvalid choice! Please select a valid option (0-21).\n");
//         }
//     }

//     // Save before exiting (if loop breaks somehow)
//     if (root_folder && auto_save_enabled) {
//         printf("\nAuto-saving before exit...\n");
//         save_filesystem(root_folder, SAVE_FILE);
//     }

//     return 0;
// }





// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "structure.h"
// #include "filesystem.h"
// #include "dir_ops.h"
// #include "file_ops.h"
// #include "resolve.h"
// #include "storagefolder.h"

// #define SAVE_FILE "filesystem.dat"

// static void trim_newline(char *str) {
//     if (!str) return;
//     size_t len = strlen(str);
//     while (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r')) {
//         str[--len] = '\0';
//     }
// }

// void display_menu() {
//     printf("\n=== FILE SYSTEM SIMULATOR ===\n");
//     printf("Current: %s\n\n", get_current_path());
//     printf("1. Create Directory\n");
//     printf("2. Create File\n");
//     printf("3. List Contents (ls)\n");
//     printf("4. Change Directory (cd)\n");
//     printf("5. Remove Directory\n");
//     printf("6. Remove File\n");
//     printf("7. Show Tree\n");
//     printf("8. Save\n");
//     printf("9. Load\n");
//     printf("0. Exit\n");
//     printf("\nChoice: ");
// }

// int main() {
//     char input[256];
//     char name[100];
//     char path[256];
//     int choice;
    
//     printf("File System Simulator\n");
//     printf("Loading saved filesystem...\n");
    
//     // Try to load existing filesystem
//     root_folder = load_filesystem(SAVE_FILE);
//     if (!root_folder) {
//         printf("No saved filesystem found. Creating new...\n");
//         init_filesystem();
//     }
//     current_folder = root_folder;
    
//     while (1) {
//         display_menu();
        
//         if (!fgets(input, sizeof(input), stdin)) break;
//         choice = atoi(input);
        
//         switch (choice) {
//             case 1: // Create Directory
//                 printf("Enter directory name: ");
//                 fgets(name, sizeof(name), stdin);
//                 trim_newline(name);
                
//                 if (strlen(name) > 0) {
//                     FOLDER *new_dir = dir_new(name, current_folder);
//                     if (new_dir) {
//                         dir_add_child(current_folder, new_dir);
//                     }
//                 }
//                 break;
                
//             case 2: // Create File
//                 printf("Enter file name: ");
//                 fgets(name, sizeof(name), stdin);
//                 trim_newline(name);
                
//                 if (strlen(name) > 0) {
//                     file_new(name, current_folder, 0);
//                 }
//                 break;
                
//             case 3: // List Contents
//                 printf("\nContents of %s:\n", current_folder->relative_path);
//                 printf("Directories:\n");
//                 FOLDER *child = current_folder->first_child;
//                 while (child) {
//                     printf("  [DIR]  %s/\n", child->name);
//                     child = child->next_sibling;
//                 }
                
//                 printf("Files:\n");
//                 FSFile *file = current_folder->files_head;
//                 while (file) {
//                     printf("  [FILE] %s\n", file->name);
//                     file = file->next;
//                 }
//                 break;
                
//             case 4: // Change Directory
//                 printf("Enter path (.. for parent, / for root): ");
//                 fgets(path, sizeof(path), stdin);
//                 trim_newline(path);
                
//                 if (strcmp(path, "..") == 0) {
//                     if (current_folder->parent) {
//                         current_folder = current_folder->parent;
//                     }
//                 } else if (strcmp(path, "/") == 0) {
//                     current_folder = root_folder;
//                 } else {
//                     FOLDER *dir = resolve_dir(path);
//                     if (dir) {
//                         current_folder = dir;
//                     } else {
//                         printf("Directory not found.\n");
//                     }
//                 }
//                 break;
                
//             case 5: // Remove Directory
//                 printf("Enter directory name: ");
//                 fgets(name, sizeof(name), stdin);
//                 trim_newline(name);
                
//                 FOLDER *to_remove = find_dir(current_folder, name);
//                 if (to_remove && to_remove != root_folder) {
//                     rmdir_dir(root_folder, name);
//                 } else {
//                     printf("Cannot remove.\n");
//                 }
//                 break;
                
//             case 6: // Remove File
//                 printf("Enter file name: ");
//                 fgets(name, sizeof(name), stdin);
//                 trim_newline(name);
                
//                 file_delete(current_folder, name);
//                 break;
                
//             case 7: // Show Tree
//                 print(root_folder, 0);
//                 break;
                
//             case 8: // Save
//                 save_filesystem(root_folder, SAVE_FILE);
//                 break;
                
//             case 9: // Load
//                 {
//                     FOLDER *loaded = load_filesystem(SAVE_FILE);
//                     if (loaded) {
//                         root_folder = loaded;
//                         current_folder = root_folder;
//                     }
//                 }
//                 break;
                
//             case 0: // Exit
//                 printf("Saving before exit...\n");
//                 save_filesystem(root_folder, SAVE_FILE);
//                 printf("Goodbye!\n");
//                 return 0;
                
//             default:
//                 printf("Invalid choice.\n");
//         }
//     }
    
//     return 0;
// }