#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "structure.h"
#include "tokenizer.h"
char** tokenize_path(const char* path, int* token_count) {
    char* path_copy = strdup(path);  // Make a copy since strtok modifies string
    char** tokens = malloc(sizeof(char*) * 100);  // Max 100 path components
    int count = 0;
    
    char* token = strtok(path_copy, "/");
    while (token != NULL && count < 100) {
        tokens[count] = strdup(token);
        count++;
        token = strtok(NULL, "/");
    }
    
    *token_count = count;
    free(path_copy);
    return tokens;
}

// Helper to free tokens
void free_tokens(char** tokens, int count) {
    for (int i = 0; i < count; i++) {
        free(tokens[i]);
    }
    free(tokens);
}
