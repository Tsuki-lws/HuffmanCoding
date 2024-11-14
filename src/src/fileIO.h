// 处理与文件的交互
#ifndef FILEIO_H
#define FILEIO_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "HuffmanTree.h"

#define BUFFER_SIZE (512*1024)
using namespace std;

// 文件头信息
struct fileHead {
    int alphaVarity; // 字符种类数量
    long long originBytes; // 源文件字节数
    int nameLength; // 文件名长度
    char name[256]; // 文件名字符数组（可根据实际文件名长度调整大小）
};

// 字母及其权值
struct alphaCode {
    char alpha;
    long long freq;
    alphaCode() {}
    alphaCode(const pair<char, long long>& x) : alpha(x.first), freq(x.second) {}
};

class FileIO{
    public:
        //压缩部分---------------------------------------------------------------

        // 读取单个文件内容并构建字符频率表
        map<char, long long> makeCharFreq(const string& filename);

        // 压缩单个文件
        void compressFile(const string& filename, const string& outputFileName);

        //解压缩部分-------------------------------------------------------------

        //读取压缩文件头信息
        pair<fileHead,streampos> readFileHead(const string& filename);

        //读取压缩文件字符频度信息,构建哈夫曼树
        pair<map<char, long long>,streampos> readCompressTFileFreq(const string& filename,
                                                        int alphaVarity,streampos currentPos);
        // 解压缩单个文件
        void decompressFile(const string& filename, string& outputFileName);

        // // 判断是否是文件夹
        // bool isDirectory(const string& filename);

        
    // private:
    //     // 压缩单个文件,输出到outputFileName
    //     void compressFile(const string& filename, const string& outputFileName);

    //     // 压缩文件夹,输出到outputFileName
    //     void compressDirectory(const string& filename, const string& outputFileName);
};

#endif