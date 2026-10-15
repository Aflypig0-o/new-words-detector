#ifndef DETECTOR_H
#define DETECTOR_H
#include <stdbool.h>
#include "wordlist.h"
#include "tokenizer.h"

// 最多记录多少个生词（四级卷子实际生词通常 200~500 个，5000 绰绰有余）
#define MAX_DETECTED 5000

// ============================================================================
// DetectedWord — 一个被检测到的生词
//
// 记录内容:
//   word    : 单词本身（小写）
//   meaning : 中文释义（从 CET4 词表带过来的）
//   line    : 在卷子中第一次出现的行号
//   col     : 在卷子中第一次出现的列号
//   c       : 在卷子中总共出现了几次
// ============================================================================
typedef struct
{
    char word[MAX_WORD_LEN];
    char meaning[MAX_MEANING_LEN];
    int line;
    int col;
    int c;
}DetectedWord;

// ============================================================================
// DetectionResult — 检测结果的集合
//
// words[] : 所有被检测到的生词（已去重）
// count   : 生词总数
// ============================================================================
typedef struct
{
    DetectedWord words[MAX_DETECTED]; 
    int count;
}DetectionResult;

// ----------------------------------------------------------------------------
// detector_find
//
// 核心检测函数：遍历所有切出的单词，在生词池中查找，
// 命中的词连同释义、行号、列号记录到 result 中，自动去重。
//
// 参数:
//   tokens : tokenize 切出的所有单词（含位置信息）
//   target : 生词池（CET4 - 高考 的差集）
//   result : 输出结果（会被填充）
// ----------------------------------------------------------------------------
void detector_find(const TokenList *tokens,const WordList *target,DetectionResult *result);

// ----------------------------------------------------------------------------
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
// ----------------------------------------------------------------------------
bool detector_save(const DetectionResult *result,const char *filename);

// ----------------------------------------------------------------------------
// detector_print
//
// 将检测结果打印到屏幕（stdout），用于调试和快速查看。
// ----------------------------------------------------------------------------
void detector_print(const DetectionResult *result);

#endif