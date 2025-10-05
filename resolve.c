#include "filesystem.h"
#include "structure.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "tokenizer.h"

FOLDER* resolve_dir(const char* path){

	if(!path || strlen(path) ==0){
		return current_folder;
	}
	FOLDER* start_folder;
	
	if(!root_folder){
			printf("Error: File system not initialized");
			return NULL;
	}
	if(path[0] == '/'){

		start_folder = root_folder;
	}else{
		start_folder = current_folder;
	}

	if(strcmp(path, "/") == 0){
		return root_folder;
	}
	
	int token_count;
	char** tokens = tokenize_path(path, &token_count);

	FOLDER* current = start_folder;
	
	for( int i = 0; i< token_count && current != NULL; i++){
		if( strcmp(tokens[i], ".") == 0){
			continue;
		}
		else if(strcmp(tokens[i], "..") == 0){
			// Parent Directory
			if (current->parent != NULL){
				current = current->parent;
			}
		}
		else{
			// Finding child folder with this name
			FOLDER* child = current->first_child;
			int found = 0;
			
			while ( child != NULL){
				if(strcmp(child->name, tokens[i]) ==0){
					current = child;
					found = 1;
					break;
				}
				child = child->next_sibling;
			}
			if(!found){
				printf("Error: Folder '%s' not found in path.\n", tokens[i]);
				free_tokens(tokens, token_count);
				return NULL;
			}
		}
	}
	
	free_tokens(tokens, token_count);
	return current;
}

FSFile* resolve_file(const char* path){
	if(!path || strlen(path) == 0){
		printf("Error: Empty file path.\n");
		return NULL;	
	}

	char* path_copy = strdup(path);
	char* last_slash = strrchr(path_copy, '/');
	
	if(last_slash == NULL){
		FOLDER* dir = current_folder;
		
		// Searching for file in current directory
		FSFile* file = dir->files_head;
		while(file!=NULL){
			if(strcmp(file->name, path_copy)==0){
				free(path_copy);
				return file;
			}
			file = file->next;
		}
		printf("Error: File '%s' not found.\n", path_copy);
		free(path_copy);
		return NULL;
	}
        // Separate directory path and filename
        *last_slash = '\0';  // Split string at the slash
        char* dir_path = path_copy;
        char* filename = last_slash + 1;
        
        // Resolve the directory
        FOLDER* dir = resolve_dir(dir_path);
        if (!dir) {
            free(path_copy);
            return NULL;
        }
        
        // Search for file in the directory
        FSFile* file = dir->files_head;
        while (file != NULL) {
            if (strcmp(file->name, filename) == 0) {
                free(path_copy);
                return file;
            }
            file = file->next;
        }
        
        printf("Error: File '%s' not found in directory.\n", filename);
        free(path_copy);
        return NULL;
}
    
