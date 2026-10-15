#ifndef WORDLIST_H
#define WORDLIST_H
#include <stdbool.h>

#define MAX_WORDS 10000
#define MAX_WORD_LEN 32
#define MAX_MEANING_LEN 128

typedef struct
{
    char word[MAX_WORD_LEN];
    char meaning[MAX_MEANING_LEN];
}WordEntry;

typedef struct
{
    WordEntry entries[MAX_WORDS];
    int count;
}WordList;

void wordlist_init(WordList *list);//初始化
bool wordlist_load(WordList *list,const char *filename);//从文件读取单词表
int wordlist_find(const WordList *list,const char *word);//判断某个词是否在单词表内
void wordlist_remove_all(WordList *big,const WordList *small);//去掉屏蔽词表中的单词
bool wordlist_save(const WordList *list,const char *filename);//保存词表
void wordlist_print(const WordList *list);//打印词表
void wordlist_sort(WordList *list);//词表排序

#endif