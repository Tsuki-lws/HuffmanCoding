//存放辅助功能和工具函数
#ifndef UTILS_H
#define UTILS_H

#include<iostream>
#include<string>
#include<sstream>

#include "features.h"
#include "fileIO.h"
#include "HuffmanTree.h"

using namespace std;

enum  {
    COMPRESSFILE = 1,
    COMPRESSDIRECTORY = 2,
    DECOMPRESSFILE = 3,
    DECOMPRESSDIRECTORY = 4
};
enum PasswordStatus {
    CORRECT = 1,
    INCORRECT = 2,
    EXIT = 3
};
// 与用户交互
int getEncodeOrDecode();
// 执行选择
void Execution(int choice);
// 简化交互
string* printInteraction(int choice);
// 获得每个文件的压缩文件的大小
long long* getCompressDirSize(const string& filename, int filenameSize);
// 是否加密
string encrypt();
// 是否解密
string decode();
// 判断密码是否正确
int passwordCorrect(const string& filename);
#endif