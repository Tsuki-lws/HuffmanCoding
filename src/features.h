// 用于存放文件夹压缩解压缩功能
#ifndef FEATURES_H
#define FEATURES_H

#include <string>
#include<filesystem>
#include<vector>


namespace fs = std::filesystem;
using namespace std;

class Features {
    public:
        // 压缩文件
        void compress(const string& filename, const string& outputFileName);

        // 解压缩文件
        void decompress(const string& filename, string& outputFileName);

        // 判断是否是文件夹
        bool isDirectory(const string& filename);
        
        // 解压缩文件夹
        void decompressDir(const string& filename);
    private:
        // 压缩单个文件
        long long compressFile(const string& filename, const string& outputFileName);

        // 压缩文件夹
        void compressDirectory(const string& dirPath, const string& outputFileName);

        // // 解压缩文件
        // void decompressFile(const string& filename, const string& outputFileName);

};

#endif