// 用于存放文件夹压缩解压缩功能
#ifndef COMPRESS_H
#define COMPRESS_H
#include <string>
#include<filesystem>
#include<vector>
#include <thread>
#include <mutex>
#include <future>
#include <algorithm>
using namespace std;
namespace fs = filesystem;


class Compress {
    public:
        // 压缩文件
        void compress(const string& filename, const string& outputFileName, const string &password);

        // 判断是否是文件夹
        bool isDirectory(const string& filename);

    private:
        // 压缩单个文件
        long long compressFile(const string& filename, const string& outputFileName, const string &prefix);

        // 压缩文件夹
        void compressDirectory(const string& dirPath, const string& outputFileName);
};;

#endif // COMPRESS