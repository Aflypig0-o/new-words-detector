#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "wordlist.h"
#include "io_utils.h"

static int split_meanings(const char *src,char pieces[][MAX_MEANING_LEN],int max_pieces)
{
    int count = 0;
    int i = 0;
    int j = 0;
    while(src[i] != '\0' && count < max_pieces)
    {
        j = 0;
        while(src[i] != '\0' && src[i] != ',' && src[i] != ';')
        {
            if(j < MAX_MEANING_LEN -1)
            {
                pieces[count][j++] = src[i];
            }
            ++i;
        }
        pieces[count][j] = '\0';
        io_trim(pieces[count]);
        if(strlen(pieces[count]) > 0)
        {
            ++count;
        }
        if(src[i] == ';' || src[i] == ',')
        {
            ++i;
        }
    }
    return count;
}

void wordlist_init(WordList *list)
{
    if(list == NULL)
    {
        return;
    }   
    list->count = 0;
}

bool wordlist_load(WordList *list,const char *filename)
{
    if(filename == NULL)
    {
        fprintf(stderr,"wordlist_load: invalid argument\n");
        return false;
    }
    FILE *fp = fopen(filename,"r");
    if(fp == NULL)
    {
        perror("fopen");
        return false;
    }
    char line[512];
    list->count = 0;
    while(fgets(line,sizeof(line),fp) != NULL)
    {
        line[strcspn(line,"\r\n")] = '\0';
        if(is_blank_line(line))
        {    
            continue;
        }
        if(list->count >= MAX_WORDS)
        {
            fprintf(stderr,"WARNING : The vocabulary list exceeds %d words; the excess has been ignored\n",MAX_WORDS);
            break;
        }
        int sep = 0;
        while(line[sep] != '\0' && !isspace((unsigned char)line[sep]))
        {
            ++sep;
        }
        int wlen = sep;
        if(wlen >= MAX_WORD_LEN)
        {
            wlen = MAX_WORD_LEN -1;
        }
        for(int i = 0;i<wlen;++i)
        {
            list->entries[list->count].word[i] = (char)tolower((unsigned char)line[i]);
        }
        list->entries[list->count].word[wlen] = '\0';
        while (line[sep] != '\0' && isspace((unsigned char)line[sep]))
            ++sep;

        // 4. 存释义（整段拷贝，没有就留空）
        if (line[sep] != '\0')
        {
            strncpy(list->entries[list->count].meaning, line + sep, MAX_MEANING_LEN - 1);
            list->entries[list->count].meaning[MAX_MEANING_LEN - 1] = '\0';
        }
        else
        {
            list->entries[list->count].meaning[0] = '\0';
        }

        list->count++;
    }
    fclose(fp);
    return true;
}

int wordlist_find(const WordList *list,const char *word)
{
    for(int i=0;i<list->count;++i)
    {
        if(strcmp(list->entries[i].word,word)==0)
        {
            return i;
        }
    }
    return -1;
}

void wordlist_remove_all(WordList *big,const WordList *small)
{
    static WordList result;   // 1.6MB，不能放栈上
    result.count = 0;
    for(int i=0;i<big->count;++i)
    {
        if(wordlist_find(small,big->entries[i].word) == -1)
        {
            result.entries[result.count] = big->entries[i];
            result.count++;
        }
    }
    *big = result;
}

bool wordlist_save(const WordList *list,const char *filename)
{
    if(filename == NULL)
    {
        fprintf(stderr,"wordlist_save: invalid argument\n");
        return false;
    }
    FILE *fp = fopen(filename,"w");
    if(fp == NULL)
    {
        perror("fopen");
        return false;
    }
    for(int i=0;i<list->count;++i)
    {
        fprintf(fp,"%s %s\n",list->entries[i].word,list->entries[i].meaning);
    }
    fclose(fp);
    return true;
}

void wordlist_print(const WordList *list)
{
    printf("共%d个单词\n",list->count);
    for(int i =0;i<list->count;++i)
    {
        printf("[%d] %s -> %s\n",i,list->entries[i].word,list->entries[i].meaning);
    }
}

void wordlist_sort(WordList *list)
{
    int n = list->count;
    for(int i=0;i<n-1;++i)
    {
        bool is_swapped = false;
        for(int j = 0;j<n-1-i;++j)
        {
            if(strcmp(list->entries[j].word,list->entries[j+1].word)>0)
            {
                WordEntry temp = list->entries[j];
                list->entries[j] = list->entries[j+1];
                list->entries[j+1] = temp;
                is_swapped = true;
            }
        }
        if(!is_swapped)
        {
            break;
        }
    }
}

void wordlist_dedup(WordList *list) {
    if (list->count <= 1) return;
    
    int j = 0;
    for (int i = 1; i < list->count; i++) {
        if (strcmp(list->entries[i].word, list->entries[j].word) == 0) {
            if (strcmp(list->entries[i].meaning, list->entries[j].meaning) == 0) {
                continue; // 完全相同，跳过
            }
            
            char pieces[30][MAX_MEANING_LEN];
            int n = split_meanings(list->entries[i].meaning, pieces, 30);
            
            for (int k = 0; k < n; k++) {
                char pattern[MAX_MEANING_LEN + 4];
                snprintf(pattern, sizeof(pattern), ";%s;", pieces[k]);
                
                char dst_check[MAX_MEANING_LEN * 2];
                snprintf(dst_check, sizeof(dst_check), ";%s;", list->entries[j].meaning);
                
                // 如果没找到，追加
                if (strstr(dst_check, pattern) == NULL) {
                    size_t cur_len = strlen(list->entries[j].meaning);
                    size_t add_len = strlen(pieces[k]);
                    
                    if (cur_len + add_len + 2 < MAX_MEANING_LEN) {
                        strncat(list->entries[j].meaning, ";", MAX_MEANING_LEN - cur_len - 1);
                        strncat(list->entries[j].meaning, pieces[k], MAX_MEANING_LEN - cur_len - 2);
                    }
                }
            }
        } else {
            j++;
            if (i != j) {
                list->entries[j] = list->entries[i];
            }
        }
    }
    list->count = j + 1;
}