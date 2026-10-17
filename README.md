# 新词检测器（New Words Detector）

## 简介

一个用 C 语言编写的命令行工具，用于从英文试卷中自动找出**超出当前词汇水平**的生词，同时具备乱序词表整理并简单去重功能。

典型场景：
- 你已经有高考 3500 词的词汇量
- 正在准备四级考试
- 做卷子时想快速知道哪些词是"四级独有的生词"
- 拿到的单词表是乱序的，想要整理一张顺序词表
- 顺序词表有重复项影响阅读

输入试卷文本，输出一份标注了释义和出现位置的生词表。
**或**
输入想要排序的乱序词表路径和存放排序后结果词表文件的路径，输出一份按照字母顺序排列好的词表，并且自动去重
**注！**词表排序前请把中文标点全部替换为英文，否则可能导致程序崩溃等一系列不可控后果

## 功能

- 从"大词表"（如四级词汇）中剔除"小词表"（如高考 3500 词）
- 自动切分试卷文本，提取所有英文单词
- 匹配生词，输出单词、释义、所在行号
- 结果保存到文件，方便打印或复习
- 新增:词表排序，不覆盖旧表
- 新增:词表去重（受限于个人能力，去重仍会出现重复翻译，请无视掉，谢谢啦）

## 项目结构

    new_words_detector/
    ├── data/                     输入数据
    |   ├── wordlist_unordered.txt默认乱序表位置
    │   ├── cet4.txt              大词表（四级）
    │   ├── gaokao3500.txt        小词表（高考）
    │   └── exam.txt              试卷文本
    ├── src/                      源代码
    │   ├── main.c                主流程
    │   ├── wordlist.c/h          词表管理
    │   ├── tokenizer.c/h         文本切分
    │   ├── detector.c/h          生词匹配
    │   └── io_utils.c/h          文件与字符串工具
    ├── output/                   输出结果
    |   ├── wordlist_sorted.txt   默认排序后词表位置
    │   ├── cet4_only.txt         剔除后的目标词表
    │   └── result.txt            最终生词表
    ├── Makefile                  编译脚本(此由AI完成)
    ├── README.md                 本文件
    └── TODO.md                   待办事项(目前被删掉了)

## 数据文件格式

### 词表文件（cet4.txt / gaokao3500.txt）

每行一个词条，格式为：

    单词  词性. 释义

示例：

    access  v. 获取  n. 接近，入口
    project  n. 工程；课题、作业
    intention  n. 打算，意图

单词和释义之间用空格或制表符分隔。只有"第一段"会被当作单词，后面全部是释义。

### 试卷文本（exam.txt）

普通英文文本，可以包含标点、数字、换行。程序会自动切分出所有连续字母序列作为单词。

示例：

    I have a project to finish.
    My intention is to access the database.
    The apple is red.

### 乱序表（wordlist_unordered.txt）

一个没有排序好的词表，默认没有任何内容，可以录入需要排序的词表，为默认排序功能输入词表位置

词条格式同上

### 顺序表（wordlist_sorted.txt）

默认排序完成后新词表输出位置

## 编译

### 方式 1：Makefile（推荐）

    mingw32-make

生成的 `detector.exe` 位于项目根目录。

### 方式 2：手动编译

    gcc src/main.c src/wordlist.c src/tokenizer.c src/detector.c src/io_utils.c -o detector.exe

## 查找生词功能使用

### 步骤 1：准备数据

把三个文件放到 `data/` 目录：

- `data/cet4.txt`：大词表
- `data/gaokao3500.txt`：小词表
- `data/exam.txt`：试卷文本

### 步骤 2：运行

 - 点开项目中detector.exe运行程序

**注意**：必须在项目根目录下运行，否则找不到 `data/` 里的文件。

### 步骤 3：查看结果

- `output/cet4_only.txt`：剔除后的目标词表
- `output/result.txt`：最终生词表

`result.txt` 示例：

    共 3 个生词：

    第 1 行  project          n. 工程；课题、作业
    第 2 行  intention        n. 打算，意图
    第 2 行  access           v. 获取  n. 接近，入口

## 排序词表功能使用

### 步骤1：导入乱序词表

 使用默认路径：
 - 将想要排序的乱序表内容填充复制到data/wordlist_unordered.txt
 使用自定义路径：
 - 将想要排序的乱序表内容填充复制到对应.txt文件内
 
### 步骤2：运行

 使用默认路径：
 - 点开项目中detector.exe运行程序
 - 按照提示使用默认路径
 使用自定义路径：
 - 点开项目中detector.exe运行程序
 - 按照提示填写自定义路径（务必写完整）
 
### 步骤3：查看结果
 使用默认路径：
 - 打开output/wordlist_sorted.txt查看排序后内容
 使用自定义路径：
 - 打开对应的.txt查看排序后内容

## 编译

### 方式 1：使用 Makefile（推荐）

    mingw32-make

生成的 `detector.exe` 位于项目根目录。

### 方式 2：手动编译

    gcc src/main.c src/wordlist.c src/tokenizer.c src/detector.c src/io_utils.c -o detector.exe

## 运行

1. 准备数据文件（放在 `data/` 目录）：
   - `data/cet4.txt`：大词表
   - `data/gaokao3500.txt`：小词表
   - `data/exam.txt`：试卷文本

2. 运行程序：

    ./detector.exe
    去重会自动运行

3. 查看结果：
   - `output/cet4_only.txt`：剔除后的目标词表
   - `output/result.txt`：最终生词表

**注意**：必须在项目根目录下运行，否则找不到 `data/` 里的文件。

## 下载

Windows 用户可以直接从 [Releases](https://github.com/Aflypig0-o/new-words-detector/releases) 下载编译好的 `detector.exe`，无需自行编译。

**下载后**：
1. 解压到任意目录
2. 在同目录创建 `data/` 文件夹，放入三个输入文件
3. 双击 `detector.exe` 运行

## 设计说明

### 查找功能数据流

    data/cet4.txt ─┐
                   ├─→ [剔除] ─→ output/cet4_only.txt ─┐
    data/gaokao.txt┘                                   │
                                                       ├─→ [匹配] ─→ output/result.txt
    data/exam.txt ──→ [切分] ──→ 单词数组 ─────────────┘

### 排序功能数据流（用默认路径演示）

    data/wordlist_unordered.txt->[排序算法排序]->[去重模块去重]->[输出到]->output/wordlist_sorted.txt

### 模块职责

| 模块 | 职责 |
| :--- | :--- |
| `io_utils` | 文件读写、字符串工具（转小写、去空白） |
| `wordlist` | 词表结构、加载、查找、剔除、保存、排序、去重|
| `tokenizer` | 文本切分，提取所有英文单词并记录行号 |
| `detector` | 匹配生词，记录位置和释义，输出结果 |

### 为什么用线性查找？

词表规模最多几千个词，线性查找（O(n)）足够快。
整个流程约 2000 万次字符串比较，现代 CPU 1 秒内跑完。
不需要哈希表或二分查找。

### 为什么使用冒泡排序？

简单好写，就这样
后续会优化排序算法

## 性能

以真实数据测试：

| 项目 | 数量 |
| :--- | :--- |
| 四级词表 | ~4500 |
| 高考词表 | ~3500 |
| 目标词表 | ~1500 |
| 试卷单词 | ~2000 |
| **运行时间** | **< 1 秒** |

## 已知限制

- **不支持词形变化**：试卷里的 `apples` 不会匹配词表里的 `apple`
- **行列记法可能错误**：受限于OCR技术缺陷，可能不会输出正确的位置
- **词表格式固定**：一行一个词条，第一段是单词，其余是释义
- **大小写不敏感**：匹配时全部转小写，但输出的是词表里的原始形式
- **可能的漏洞**：受限于作者可怜而薄弱的C语言基础（截止到该项目发布，作者从零开始才学习了40天C语言）可能会存在漏洞，还请见谅
- **去重重复翻译** 受限于作者能力不足，还需要进一步改进

## 反馈

- 使用过程中如果发现不合理之处或者任何的bug，请与作者联系，感谢支持！

## 作者

Aflypig0_o

## 参考词表来源

https://github.com/KyleBing/english-vocabulary

## 仓库

https://github.com/Aflypig0-o/new-words-detector

## 日期

v0.1.0版本：2026-10-14
v0.2.0版本：2026-10-15
v0.3.0版本：2026-10-17
