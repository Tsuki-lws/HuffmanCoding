//存放辅助功能和工具函数
#ifndef UTILS_H
#define UTILS_H

#include<string>
#include<vector>
#include<iostream>
#include<sstream>
#include <fstream>
#include <filesystem>
using namespace std;
namespace fs = filesystem;
enum coverStatus{
    SKIP = 0,
    OVERWRITE = 1,
    OTHER = 2,
    EXIT = 3
};
// 获得每个文件的压缩文件的大小
long long* getCompressDirSize(const string& filename, int filenameSize);
// 检查输出路径是否存在,解压缩
int checkOutputPath(const string &filepath);
// 相同文件数过多，确认是否全部覆盖
int coverAll(const vector<string> &filepath, int filenameSize);
// 检查输出路径是否存在,压缩
bool checkCompressOutputPath(const string &filepath);
#endif // UTILS_H