#ifndef TOKENIZER	
#define TOKENIZER	
char** tokenize_path(const char* path, int* token_count);
void free_tokens(char** tokens, int count);
#endif
