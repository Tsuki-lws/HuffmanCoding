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
// 与用户交互
int getEncodeOrDecode();
// 执行选择
void Execution(int choice);
// 简化交互
string* printInteraction(int choice);
#endif