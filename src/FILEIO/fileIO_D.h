// 处理与文件的交互
#ifndef FILEIO_D_H
#define FILEIO_D_H

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

class FileIO_D{
    public:
        //读取压缩文件头信息
        pair<fileHead,streampos> readFileHead(const string& filename,const streampos &startIndex);

        // 解压缩单个文件
        void decompressFile(const string& filename,const string& outputFileName,
                                        long long filesize,const streampos &startIndex);

        // 读取哈夫曼树并返回树的根节点
        HuffmanNode* readHuffmanTree(ifstream& file, long size);
    private:
        // 解压缩块
        vector<char> decompressBlock(const char* inputBuffer, int size,HuffmanNode *current);
        
        // 从文件中读取哈夫曼树
        HuffmanNode* readTreeFromBuffer(ifstream& file, vector<char> &buffer, int& inputIndex, 
                                                        int &bitcount, char &bits, long &remainingSize);
        // 读取移位
        bool readGresson(char &bits, int &bitcount, int& inputIndex, vector<char> &buffer, 
                                            long &remainingSize, ifstream& file, bool &data);
        // 多线程压缩
        void decompressWithMultiThread(ifstream &inputFile, ofstream &outputFile,
                    HuffmanTree &tree, HuffmanNode *root,long long filesize, streampos newPos, long long originBytes);
        // 单线程压缩
        void decompressWithSingleThread(ifstream &inputFile, ofstream &outputFile,HuffmanTree &tree, 
                                        HuffmanNode *root, long long filesize, streampos headSize, long long originBytes);                                    
};          

#endif