#include<time.h>
#include<stddef.h>
typedef struct metadata{
	time_t created_time;
	time_t modified_time;
	size_t size;
	// for gui - something for is_hidden or is_visible
}METADATA;	
typedef struct Folder{
	char name[100];
	struct Folder *parent;
	
	// Children folders - 
	struct Folder *first_child;
	struct Folder *last_child;
	struct Folder *prev_sibling;
	struct Folder *next_sibling;
	
	// Files in this folder -
	struct File *files_head;
	struct File *files_tail;
	
	// Metadata - 
	struct Metadata *metadata;
	
	//Extra information - 
	int file_count;
	int folder_count;
	int child_count = file_count + folder_count;
	char relative_path[250];
} FOLDER;

typedef struct File {
	char name[100];
	struct Folder *parent_folder;
	struct File *prev;
	struct File *next;
	struct Metadata *metadata;
} FSFile; 

