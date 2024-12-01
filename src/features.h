// 用于存放文件夹压缩解压缩功能
#ifndef FEATURES_H
#define FEATURES_H

#include <string>
#include<filesystem>
#include<vector>
#include <thread>
#include <mutex>
#include <future>
#include <algorithm>
namespace fs = std::filesystem;
using namespace std;

class Features {
    public:
        // 压缩文件
        void compress(const string& filename, const string& outputFileName, const string &password);

        // 解压缩文件
        void decompress(const string& filename, string& outputFileName, int passLength);

        // 判断是否是文件夹
        bool isDirectory(const string& filename);

    private:
        // 压缩单个文件
        long long compressFile(const string& filename, const string& outputFileName, const string &prefix);

        // 压缩文件夹
        void compressDirectory(const string& dirPath, const string& outputFileName);

        // 解压缩文件
        void decompressFile(const string& filename, string& outputFileName, streampos currentPos);

        // 解压缩文件夹
        void decompressDir(const string& filename, streampos currentPos);
        
        // 用于多线程
        void decompressFileTask(const string& filename, string& filepath, int filesize, int startIndex);
};

#endif