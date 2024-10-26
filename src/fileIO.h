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
    alphaCode(const pair<char, long long>& x) : alpha(x.first), freq(x.second) {}
};

class fileIO{
    public:
        string inputFileName;
        string outputFileName;
        fileIO(string input,string output):inputFileName(input),outputFileName(output){}
        
        // 读取单个文件内容并构建字符频率表
        map<char, long long> readFile();

        // 压缩文件
        void compressFile();

        //读取压缩文件头信息
        fileHead readFileHead();

        //读取压缩文件字符频度信息,构建哈夫曼树
        map<char, long long> readCompressTFileFreq(int alphaVarity);

        // 解压缩文件
        void decompressFile();

}

#endif