#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include "wordlist.h"
#include "tokenizer.h"
#include "detector.h"

static void run_detector()
{
    // 1. 加载两个词表
    static WordList big;       // 大表（四级）
    static WordList little;     // 小表（高考）
    static WordList target;    // 目标表（剔除后）

    if (!wordlist_load(&big, "data/cet4.txt")) {
        fprintf(stderr, "加载 cet4.txt 失败\n");
        return;
    }
    if (!wordlist_load(&little, "data/gaokao3500.txt")) {
        fprintf(stderr, "加载 gaokao3500.txt 失败\n");
        return;
    }

    printf("大表：%d 个词\n", big.count);
    printf("小表：%d 个词\n", little.count);

    //2. 剔除
    target = big;                       // 结构体整体赋值
    wordlist_remove_all(&target, &little);
    printf("目标词表：%d 个词\n", target.count);

    wordlist_save(&target, "output/cet4_only.txt");

    //3. 切分试卷
    static TokenList tokens;
        if (!tokenize_file("data/exam.txt", &tokens)) {
        fprintf(stderr, "加载 exam.txt 失败\n");
        return;
    }
    printf("试卷：%d 个单词\n", tokens.count);

    //4. 匹配
    static DetectionResult result;
    detector_find(&tokens, &target, &result);
    printf("找到 %d 个生词\n", result.count);

    //5. 输出
    detector_print(&result);
    detector_save(&result, "output/result.txt");
        printf("\n按任意键退出...");
    _getch();
}

static void run_sort()
{
    char input_file[256];
    char output_file[256];

    printf("请输入输入文件完整路径(默认在data/wordlist_unordered.txt,留空则考虑默认):\n");
    printf("***注意,wordlist_unordered.txt文件内部默认留空,请自行填充内容***\n");
    fgets(input_file,sizeof(input_file),stdin);
    input_file[strcspn(input_file,"\n")] = '\0';
    if(input_file[0] == '\0')
    {
        strcpy(input_file,"data/wordlist_unordered.txt");
    }
    printf("使用%s\n",input_file);
    
    printf("请输入输出文件完整路径(默认在output/wordlist_sorted.txt,留空则考虑默认):\n");
    fgets(output_file,sizeof(output_file),stdin);
    output_file[strcspn(output_file,"\n")] = '\0';
    if(output_file[0] == '\0')
    {
        strcpy(output_file,"output/wordlist_sorted.txt");
    }
    printf("使用%s\n",output_file);

    if(strcmp(input_file,output_file) == 0)
    {
        fprintf(stderr,"WARNING: 输入和输出不能为同一个文件\n");
        return;
    }
    static WordList list;
    wordlist_init(&list);
    if(!wordlist_load(&list,input_file))
    {
        fprintf(stderr,"ERROR: 加载%s失败\n",input_file);
        printf("可能的原因:\n");
        printf(" - 文件不存在\n");
        printf(" - 错误路径\n");
        printf(" - 没有读取权限\n");
        return;
    }
    printf("加载%d个单词\n",list.count);
    wordlist_sort(&list);
    printf("排序完成\n");
    if(!wordlist_save(&list,output_file))
    {
        fprintf(stderr,"ERROR: 保存到%s失败\n",output_file);
        return;
    }
    printf("已经保存到%s\n",output_file);
}

int main(void)
{
    // 切换控制台输出编码为 UTF-8，确保中文不乱码
    SetConsoleOutputCP(65001);
    int choice;
    while(1)
    {
        printf("===词表工具v0.2.0,持续更新中===\n");
        printf("1.检测试卷生词\n");
        printf("2.乱序词表排序\n");
        printf("0.退出\n");
        printf("请选择(输入一个数字):\n");
        scanf("%d",&choice);
        getchar();
        switch(choice)
        {
            case 0:
            {
                printf("按任意键退出\n");
                _getch();
                return 0;
            }
            case 1:
            {
                run_detector();
                continue;
            }
            case 2:
            {
                run_sort();
                continue;
            }
            default:
            {
                printf("无效的选择,请再试一次\n");
            }
        }
    }
    return 0;
}