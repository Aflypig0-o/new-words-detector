#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "tokenizer.h"
#include "io_utils.h"

void tokenize(const char *text,TokenList *out)
{
    out->count = 0;
    int i = 0;
    int line = 1;
    int col = 1;
    while(text[i] != '\0')
    {
        if(text[i] == '\n' || text[i] == '\r')
        {
            ++line;
            col = 1;
            ++i;
            continue;
        }
        if(!isalpha((unsigned char)(text[i])))
        {
            ++col;
            ++i;
            continue;
        }
        int start = i;
        int start_col = col;
        while(text[i] != '\0' && isalpha((unsigned char)text[i]))
        {
            ++col;
            ++i;
        }
        int len = i - start;
        if (len >= MAX_TOKEN_LEN)
        {
            fprintf(stderr, "WARNING: token too long at %d, skipped\n", start);
            continue;    // 只跳过这一个
        }
        if (out->count >= MAX_TOKENS)
        {
            fprintf(stderr, "WARNING: token limit reached\n");
            break;       // 真的满了才停
        }
        for(int j=0;j<len;++j)
        {
            out->tokens[out->count][j] = (char)tolower((unsigned char)text[start+j]);
        }
        out->tokens[out->count][len] = '\0';
        out->positions[out->count] = start;
        out->lines[out->count] = line;
        out->cols[out->count] = start_col;
        out->count++;
    }
}

bool tokenize_file(const char *filename,TokenList *out)
{
    if(filename == NULL)
    {
        fprintf(stderr,"tokenize_file: invalid argument\n");
        return false;
    }
    FILE *fp = fopen(filename,"r");
    if(fp == NULL)
    {
        perror("fopen");
        return false;
    }
    long long size = get_file_size(filename);
    if(size<0)
    {
        fclose(fp);
        return false;
    }
    char *buf = (char*)malloc(size+1);
    if(buf == NULL)
    {
        perror("malloc");
        fclose(fp);
        return false;
    }
    size_t read_count = fread(buf,1,size,fp);
    buf[read_count] = '\0';
    fclose(fp);
    tokenize(buf,out);
    free(buf);
    return true;
}
