// ============================================================================
// io_utils.c - 文件 IO 和字符串工具函数的实现
//
// 提供:
//   - io_read_file  : 一次性读入整个文件到动态分配的字符串
//   - io_to_lower   : 字符串原地转小写
//   - io_trim       : 去掉字符串首尾空白
//
// 注意: 头文件中还声明了 io_read_lines（按行读文件并回调），
//       目前 .c 中尚未实现，需要时补充即可。
// ============================================================================

#include <ctype.h>    // isspace, tolower —— 字符分类/转换函数
#include <string.h>   // strlen, memmove —— 字符串/内存操作
#include "io_utils.h"

// ----------------------------------------------------------------------------
// 读文件到动态分配的字符串
//
// 参数:
//   filename : 要读取的文件路径
//
// 返回值:
//   - 成功: 指向堆上分配的字符串，调用者负责 free()
//   - 失败: NULL（文件不存在 / 打开失败 / 内存分配失败）
//
// 实现思路（待补全）:
//   1. fopen 打开文件
//   2. fseek + ftell 获取文件大小
//   3. malloc 分配 size + 1 字节（+1 留给 '\0'）
//   4. fread 一次读入
//   5. 补 '\0'，关闭文件，返回指针
//
// 注意: 当前实现未完成，下面这行有 bug：
//       第一个参数应为 filename 而非 fp
// ----------------------------------------------------------------------------
/*char *io_read_file(const char *filename)
{
    FILE *fp = fopen(filename,"r");
}*/

// ----------------------------------------------------------------------------
// 转小写（原地修改）
//
// 参数:
//   s : 要转换的字符串（必须是可写的内存，如 char[] 或 malloc 出的缓冲区）
//
// 行为:
//   - 将 s 中所有 ASCII 大写字母（A-Z）转为对应小写（a-z）
//   - 其他字符（小写、数字、标点等）由 tolower 原样返回，不受影响
//
// 关键细节:
//   - tolower() 的 C 标准要求参数类型为 unsigned char 或 EOF
//     直接传 char 时，在 char 为有符号类型（x86 等大多数平台）的情况下，
//     遇到 0x80~0xFF 的字节（例如 UTF-8 中文字符的字节）会先被符号扩展
//     为负数，再传给 tolower —— 这是未定义行为
//     所以这里先强转 unsigned char，确保传入 [0, 255] 的合法范围
//     转换结果再强转回 char 写回（结果在 ASCII 范围内，不会有歧义）
//   - 不做 UTF-8 / Unicode 感知，只按单字节处理
// ----------------------------------------------------------------------------
void io_to_lower(char *s)
{
    for(int i=0;s[i] != '\0';++i)
    {
        //tolower() 要求参数是 unsigned char 或 EOF，直接传 char 在遇到某些值会未定义行为
        //此处为了安全用unsigned char
        s[i] = (char)tolower((unsigned char)s[i]);
    }
}

// ----------------------------------------------------------------------------
// 去掉字符串首尾空白（原地修改）
//
// 参数:
//   s : 要修剪的字符串（必须是可写的内存）
//
// 行为:
//   - 去掉 s 开头的所有空白字符（空格、\t、\n、\r、\v、\f）
//   - 去掉 s 结尾的所有空白字符
//   - 中间的有效内容通过 memmove 移到字符串开头，补 '\0'
//
// 示例:
//   "  hello  "  ->  "hello"
//   "   "        ->  ""
//   "a b c "     ->  "a b c"
//
// 实现分三步:
//   1. 找 start —— 第一个非空白字符的下标
//   2. 找 end   —— 最后一个非空白字符的下标 + 1（即有效区间的右边界）
//   3. memmove 把 [start, end) 搬到开头，补 '\0'
// ----------------------------------------------------------------------------
#include <string.h>
#include <ctype.h>

void io_trim(char *s)
{
    if (s == NULL) return;

    // 第一步：去掉开头空白
    int start = 0;
    while (s[start] && isspace((unsigned char)s[start])) {
        ++start;
    }

    // 第二步：去掉结尾空白
    int end = strlen(s);
    while (end > start && isspace((unsigned char)s[end - 1])) {
        --end;
    }

    // 第三步：计算有效长度，并移动/截断字符串
    int len = end - start;
    
    if (start > 0) {
        // 把有效字符移到字符串开头
        memmove(s, s + start, len);
    }
    // 最后一定要加上结束符，否则字符串就没尾巴了
    s[len] = '\0'; 
}

 bool is_blank_line(const char *s)
{
        for(;*s;++s)
        {
            if(!isspace((unsigned char)*s))
            {
                return false;
            }
        }
        return true;
}

long long get_file_size(const char *path)
{
    FILE *fp = fopen(path,"rb");
    if(fp == NULL)
    {
        perror("fopen");
        return -1;
    }
    if(fseek(fp,0,SEEK_END)!= 0)
    {
        fclose(fp);
        return -1;
    }
    long size = ftell(fp);
    fclose(fp);
    if(size < 0 )
    {
        return -1;
    }
    return (long long)size;
}

