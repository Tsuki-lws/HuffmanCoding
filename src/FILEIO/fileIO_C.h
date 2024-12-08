// 处理与文件的交互
#ifndef FILEIO_C_H
#define FILEIO_C_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include "HuffmanTree.h"
#include <thread>
#include <filesystem>


using namespace std;

enum FileType {
    FILE_TYPE = 1,
    DIRECTORY = 0
};
// 文件头信息,单个文件
struct fileHead {
    int alphaVarity; // 字符种类数量
    long long originBytes; // 源文件字节数
    fileHead() : originBytes(0), alphaVarity(0){}
};
// 字母及其编码
struct alphaCode {
    char alpha;
    long long freq;
    alphaCode() {}
    alphaCode(const pair<char, long long>& x) : alpha(x.first), freq(x.second) {}
};

class FileIO_C{
    public:
        // 读取单个文件内容并构建字符频率表
        map<char, long long> makeCharFreq(const string& filename);

        // 压缩单个文件
        void compressFile(const string& filename, const string& outputFileName, const string &prefix);

        // 写入哈夫曼树结构
        void writeHuffmanTree(ofstream& file, HuffmanNode* root);

    private:
        // 处理空文件
        void handleEmptyFile(const string &filename, const string &outputFileName, const string &prefix,
                                                                     ifstream &inputFile,ofstream &outputFile);

        // 处理非空文件
        string* handleNonEmptyFileHead(const string &filename,
                    const string &outputFileName, const string &prefix, ifstream &inputFile,ofstream &outputFile);

        // 压缩块
        vector<char> compressBlock(const char *inputBuffer, int size, const string *charCodeArray);

        // 正常压缩文件
        void compressSmallFile(ifstream &inputFile, ofstream &outputFile, string *charCodeArray, long long filesize);

        // 多线程压缩文件
        void compressLargeFile(ifstream &inputFile, ofstream &outputFile, string *charCodeArray, long long filesize);

        // 处理缓冲区
        void processBuffer(char *inputBuffer, size_t bufferSize, string *charCodeArray, char &bits, int &bitcount
                                                        , int &outputIndex, char *outputBuffer, ofstream &outputFile);

        // 正常移位
        void gresson(char &bits, int &bitcount, int& outputIndex,char* buffer,ofstream& file,bool data);
        // 多线程移位
        void gresson(char &bits, int &bitcount, vector<char>& buffer,bool data);
        // 前序遍历存哈夫曼树到缓冲区
        void writeTreeToBuffer(ofstream& file, HuffmanNode* root, char* buffer, 
                                                            int& outputIndex, int &bitcount, char &bits);
        
};          

#endif