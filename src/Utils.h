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

enum coverStatus{
    SKIP = 0,
    OVERWRITE = 1,
    OTHER = 2,
    EXIT = 3
};
// 启动程序,与用户交互
void start();
// 获得每个文件的压缩文件的大小
long long* getCompressDirSize(const string& filename, int filenameSize);
// 是否解密
string decode();
// 判断密码是否正确
int passwordCorrect(const string& filename);
// 检查输出路径是否存在,解压缩
int checkOutputPath(const string &filepath);
// 相同文件数过多，确认是否全部覆盖
int coverAll(const vector<string> &filepath, int filenameSize);
// 检查输出路径是否存在,压缩
bool checkCompressOutputPath(const string &filepath);

// 帮助菜单
void help();
// 欢迎界面
void welcome();
#endif