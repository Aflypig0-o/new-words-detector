#ifndef TOKENIZE_H
#define TOKENIZE_H
#include <stdbool.h>

#define MAX_TOKENS 10000
#define MAX_TOKEN_LEN 32

typedef struct
{
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
    int positions[MAX_TOKENS];
    int lines[MAX_TOKENS];
    int cols[MAX_TOKENS];
    int count;
}TokenList;

void tokenize(const char *text,TokenList *out);

bool tokenize_file(const char *filename,TokenList *out);

#endif