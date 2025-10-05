#ifndef STRUCTURE_H
#define STRUCTURE_H
//#include <time.h>
//#include <stddef.h>
//
//typedef struct metadata
//{
//    time_t created_time;
//    time_t modified_time;
//    size_t size; // in bytes
//}METADATA;
//
//typedef struct Folder
//{
//    char name[100];
//    struct Folder* parent;// points to the parent of the folder
//
//    //Children Folders(siblings in a linked list)
//    struct Folder* first_child; //First child in the generation(head/front)
//    struct Folder* last_child;  //Last child in the generation(tail/back)
//    struct Folder* prev_sibling; //Previous sibling in the generation
//    struct Folder* next_sibling; //Next sibling in the generation
//
//    struct File* files_head; //First file in the folder(head/front)
//    struct File* files_tail; //Last file in the folder(tail/back)
//
//    METADATA metadata;
//
//    int file_count; //Number of files in the folder
//    int folder_count; //Number of sub-folders in the folder
//    char relative_path[500]; //Relative path from the root folder
//}FOLDER;
//
//typedef struct File
//{
//    char name[100];
//    struct Folder* parent_folder; //Points to the parent folder of the file
//
//    struct File* prev; //Previous file in the folder
//    struct File* next; //Next file in the folder
//
//    METADATA metadata;
//
//    char relative_path[500]; //Relative path from the root folder
//} FSFile;
#endif
