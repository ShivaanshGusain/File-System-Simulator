#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_content.h"

#define CONTENT_FILE "file_contents.txt"
#define MAX_TEXT 1000

typedef struct FileText {
    char filepath[500];
    char* content;
    struct FileText* next;
} FileText;

static FileText* text_list = NULL;

void write_text_to_file(FSFile* file) {
    if (!file) return;
    
    printf("Enter text (type END on new line to finish):\n");
    
    char buffer[MAX_TEXT] = "";
    char line[100];
    
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0) break;
        
        if (strlen(buffer) + strlen(line) < MAX_TEXT) {
            strcat(buffer, line);
        }
    }
    
    // Find or create entry
    FileText* curr = text_list;
    FileText* prev = NULL;
    
    while (curr) {
        if (strcmp(curr->filepath, file->relative_path) == 0) {
            // Update existing
            free(curr->content);
            curr->content = strdup(buffer);
            printf("Text saved to %s\n", file->name);
            save_file_contents();  // Auto-save
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    
    // Create new entry
    FileText* new_text = (FileText*)malloc(sizeof(FileText));
    strcpy(new_text->filepath, file->relative_path);
    new_text->content = strdup(buffer);
    new_text->next = text_list;
    text_list = new_text;
    
    printf("Text saved to %s\n", file->name);
    save_file_contents();  // Auto-save
}

void read_text_from_file(FSFile* file) {
    if (!file) return;
    
    FileText* curr = text_list;
    while (curr) {
        if (strcmp(curr->filepath, file->relative_path) == 0) {
            printf("\n--- %s ---\n", file->name);
            printf("%s", curr->content);
            printf("--- End ---\n");
            return;
        }
        curr = curr->next;
    }
    
    printf("File %s has no text content.\n", file->name);
}

void save_file_contents(void) {
    FILE* fp = fopen(CONTENT_FILE, "w");
    if (!fp) return;
    
    FileText* curr = text_list;
    while (curr) {
        fprintf(fp, "FILE:%s\n", curr->filepath);
        fprintf(fp, "%s", curr->content);
        fprintf(fp, "\nENDFILE\n");
        curr = curr->next;
    }
    
    fclose(fp);
}

void load_file_contents(void) {
    FILE* fp = fopen(CONTENT_FILE, "r");
    if (!fp) return;
    
    char line[500];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "FILE:", 5) == 0) {
            // Get filepath
            char filepath[500];
            strcpy(filepath, line + 5);
            filepath[strlen(filepath)-1] = '\0';  // Remove newline
            
            // Read content until ENDFILE
            char content[MAX_TEXT] = "";
            while (fgets(line, sizeof(line), fp)) {
                if (strcmp(line, "ENDFILE\n") == 0) break;
                strcat(content, line);
            }
            
            // Create entry
            FileText* new_text = (FileText*)malloc(sizeof(FileText));
            strcpy(new_text->filepath, filepath);
            new_text->content = strdup(content);
            new_text->next = text_list;
            text_list = new_text;
        }
    }
    
    fclose(fp);
}