// 用于存放文件夹压缩解压缩功能
#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <string>
#include<filesystem>
#include<vector>
#include <thread>
#include <mutex>
#include <future>
#include <algorithm>
using namespace std;
namespace fs = filesystem;

class Decompress  {
    public:
        // 解压缩文件
        void decompress(const string& filename, string& outputFileName, int passLength);

    private:
        // 解压缩文件
        void decompressFile(const string& filename, string& outputFileName, streampos currentPos);

        // 解压缩文件夹
        void decompressDir(const string& filename, const string &prefix, streampos currentPos);
        
        // 用于多线程
        void decompressFileTask(const string& filename, string& filepath, int filesize, int startIndex);
};

#endif // DECOMPRESS