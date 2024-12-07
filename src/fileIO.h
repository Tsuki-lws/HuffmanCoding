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
#define BLOCK_SIZE (5*1024*1024)
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

        // 写入哈夫曼树结构并记录文件大小
        void writeHuffmanTree(ofstream& file, HuffmanNode* root);

        // 读取哈夫曼树并返回树的根节点
        HuffmanNode* readHuffmanTree(ifstream& file, long size);
    private:
        // 处理空文件
        void handleEmptyFile(const string &filename, const string &outputFileName, const string &prefix,
                                                                     ifstream &inputFile,ofstream &outputFile);

        // 处理非空文件
        string* handleNonEmptyFileHead(const string &filename,
                    const string &outputFileName, const string &prefix, ifstream &inputFile,ofstream &outputFile);

        // 压缩块
        vector<char> compressBlock(const char *inputBuffer, int size, const string *charCodeArray);

        // 压缩小文件
        void compressSmallFile(ifstream &inputFile, ofstream &outputFile, string *charCodeArray, long long filesize);

        // 多线程压缩大文件
        void compressLargeFile(ifstream &inputFile, ofstream &outputFile, string *charCodeArray, long long filesize);

        // 处理缓冲区
        void processBuffer(char *inputBuffer, size_t bufferSize, string *charCodeArray, char &bits, int &bitcount, int &outputIndex, char *outputBuffer, ofstream &outputFile);

        // 解压缩块
        vector<char> decompressBlock(const char* inputBuffer, int size,HuffmanNode *current);

        
        // 处理移位
        void gresson(char &bits, int &bitcount, int& outputIndex,char* buffer,ofstream& file,bool data);
        // void gresson(char &bits, int &bitcount, int& outputIndex,char* buffer,ofstream& file,bool data);
        // 前序遍历存哈夫曼树到缓冲区
        void writeTreeToBuffer(ofstream& file, HuffmanNode* root, char* buffer, 
                                                            int& outputIndex, int &bitcount, char &bits);
        
        HuffmanNode* readTreeFromBuffer(ifstream& file, vector<char> &buffer, int& inputIndex, 
                                                        int &bitcount, char &bits, long &remainingSize);
        // 读取移位
        bool readGresson(char &bits, int &bitcount, int& inputIndex, vector<char> &buffer, 
                                            long &remainingSize, ifstream& file, bool &data);
};          
#endif