// 处理与文件的交互
#ifndef FILEIO_H
#define FILEIO_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include "HuffmanTree.h"
#include <thread>
#define BUFFER_SIZE (512*1024)
#define FILE_SIZE (10*1024*1024)
using namespace std;

enum FileType {
    FILE_TYPE = 1,
    DIRECTORY = 0
};
// 文件头信息,单个文件
struct fileHead {
    int alphaVarity; // 字符种类数量
    long long originBytes; // 源文件字节数
    // int nameLength; // 文件名长度
    // char* name; // 文件名字符数组
     fileHead() : originBytes(0), alphaVarity(0){}
};
// 字母及其编码
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
        void compressFile(const string& filename, const string& outputFileName, const string &prefix);
        //解压缩部分-------------------------------------------------------------

        //读取压缩文件头信息
        pair<fileHead,streampos> readFileHead(const string& filename,const streampos &startIndex);

        //读取压缩文件字符频度信息,构建哈夫曼树
        pair<map<char, long long>,streampos> readCompressTFileFreq(const string& filename,
                                                        int alphaVarity,streampos currentPos);
        // 解压缩单个文件
        streampos decompressFile(const string& filename,const string& outputFileName,
                                        long long filesize,const streampos &startIndex);

        
    private:
        // 处理空文件
        void handleEmptyFile(const string &filename, const string &outputFileName, const string &prefix,
                                                                     ifstream &inputFile,ofstream &outputFile);

        // 处理非空文件
        string* handleNonEmptyFileHead(const string &filename,
                    const string &outputFileName, const string &prefix, ifstream &inputFile,ofstream &outputFile);

        // 压缩块
        vector<char> compressBlock(const char *inputBuffer, int size, const string *charCodeArray);

        // 解压缩块
        vector<char> decompressBlock(const char* inputBuffer, int size,HuffmanNode *current);
};
#endif