#include <stdio.h>
#include <string.h>
#include "detector.h"

// ============================================================================
// detector_find
//
// 核心检测函数：遍历所有切出的单词，在生词池（target）中查找，
// 命中的词连同释义、行号、列号记录到 result 中，自动去重。
//
// 算法:
//   1. 遍历 tokens 的每个单词
//   2. 用 wordlist_find 在 target（生词池）中查
//      - 没查到 → 不是生词，跳过
//      - 查到了 → 进入去重逻辑
//   3. 去重：线性扫 result，看这个词是不是已经记过了
//      - 记过了 → 出现次数 c +1
//      - 没记过 → 新增一条记录（词 + 释义 + 行 + 列 + c=1）
//
// 参数:
//   tokens : tokenize 切出的所有单词（含位置信息）
//   target : 生词池（CET4 - 高考 的差集）
//   result : 输出结果（会被填充）
//
// 注意:
//   - 同一个词只记录第一次出现的位置，后续出现只累加 c
//   - 超过 MAX_DETECTED 个生词时停止并打印警告
// ============================================================================
void detector_find(const TokenList *tokens,const WordList *target,DetectionResult *result)
{
    result->count = 0;

    for(int i=0;i<tokens->count;++i)
    {
        // 第一步：在生词池里查这个词
        int idx = wordlist_find(target,tokens->tokens[i]);
        if(idx < 0)
        {
            // 不在生词池里，跳过
            continue;
        }
        else
        {
            // 第二步：去重 —— 看 result 里是不是已经记过这个词
            int existing = -1;
            for(int j=0;j<result->count;++j)
            {
                if(strcmp(result->words[j].word,tokens->tokens[i]) == 0)
                {
                    existing = j;
                    break;
                }
            }

            if(existing != -1)
            {
                // 已经记过了 → 只累加出现次数
                result->words[existing].c++;
            }
            else
            {
                // 第一次出现 → 新增记录
                if(result->count >= MAX_DETECTED)
                {
                    fprintf(stderr,"WARNING: detection limit %d reached\n", MAX_DETECTED);
                    break;
                }

                // 从生词池拷贝词和释义
                strcpy(result->words[result->count].word,target->entries[idx].word);
                strcpy(result->words[result->count].meaning,target->entries[idx].meaning);

                // 记录第一次出现的位置
                result->words[result->count].line = tokens->lines[i];
                result->words[result->count].col = tokens->cols[i];
                result->words[result->count].c = 1;

                result->count++;
            }
        }
    }
}

// ============================================================================
// detector_output
//
// 将检测结果格式化输出到 FILE 流（可以是 stdout 或文件）。
// 这是 detector_save 和 detector_print 的公共底层。
//
// 参数:
//   result : 检测结果
//   fp     : 输出目标（stdout / 文件指针）
//
// 输出格式:
//   共 N 个生词:
//
//   第X行 第Y列 单词           释义
//   ...
// ============================================================================
static void detector_output(const DetectionResult *result,FILE *fp)
{
    fprintf(fp,"共%d个生词:\n\n",result->count);
    for(int i=0;i<result->count;++i)
    {
        // %-15s 让单词列固定 15 字符宽，释义对齐
        fprintf(fp,"第%d行 第%d列 %-15s %s\n",result->words[i].line,result->words[i].col,result->words[i].word,result->words[i].meaning);
    }
}

// ============================================================================
// detector_save
//
// 将检测结果保存到文件。
//
// 参数:
//   result   : 检测结果
//   filename : 目标文件路径
//
// 返回:
//   true  — 成功
//   false — 文件打不开 / 参数为 NULL
// ============================================================================
bool detector_save(const DetectionResult *result,const char *filename)
{
    if(filename == NULL)
    {
        fprintf(stderr,"detector_save: invalid argument\n");
        return false;
    }

    FILE *fp = fopen(filename,"w");
    if(fp == NULL)
    {
        perror("fopen");
        return false;
    }

    // 复用公共输出函数，目标流为文件
    detector_output(result,fp);
    fclose(fp);
    return true;
}

// ============================================================================
// detector_print
//
// 将检测结果打印到屏幕（stdout），用于调试和快速查看。
// 内部复用 detector_output，目标流为 stdout。
// ============================================================================
void detector_print(const DetectionResult *result)
{
    detector_output(result,stdout);
}

