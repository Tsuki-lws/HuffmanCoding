#ifndef FILEIO_H
#define FILEIO_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "HuffmanTree.h"

using namespace std;

// 文件头信息
struct fileHead {
    int alphaVarity; // 字符种类数量
    long long originBytes; // 源文件字节数
};

// 字母及其权值
struct alphaCode {
    char alpha;
    long long freq;
    alphaCode() {}
    alphaCode(pair<char, long long>& x) : alpha(x.first), freq(x.second) {}
};

// 读取单个文件内容并构建字符频率表
map<char, long long> readFile(const string& filename);

// 压缩文件
void compressFile(const string& inputFilename, const string& outputFilename);

// 解压缩文件
void decompressFile(const string& inputFilename, const string& outputFilename);

#endif