#include "features.h"
#include "fileIO.h"
#include "Utils.h"
// 压缩
void Features::compress(const string &filename, const string &outputFileName)
{
    // 判断是文件还是文件夹
    if (isDirectory(filename))
    {
        // 是文件夹
        compressDirectory(filename, outputFileName);
    }
    else
    {
        // 是文件
        compressFile(filename, outputFileName);
    }
}

long long Features::compressFile(const string &filename, const string &outputFileName)
{
    long long size;
    // 判断是否存在目标文件
    fs::path str(outputFileName);
    fs::directory_entry entry(str);
    // 如果无目标文件,则其大小就是0
    if (entry.status().type() == fs::file_type::not_found)
        size = 0;
    else
        size = file_size(str);
    FileIO fileIO;
    map<char, long long> charFreq = fileIO.makeCharFreq(filename);
    fileIO.compressFile(filename, outputFileName);
    // 返回压缩单个文件的大小
    long long aftersize = file_size(str);
    long long result = aftersize - size;
    return result;
}
// 压缩文件夹
void Features::compressDirectory(const string &dirPath, const string &outputFileName)
{
    // 获取目录下的文件夹信息以及文件信息
    vector<string> dirname;
    vector<string> filename;

    // 将原文件夹路径完整的记录下来
    dirname.push_back(dirPath);
    // 遍历这个dirPath文件夹,将文件加入到filename，将文件夹加入到dirname,相对路径了
    // 这里存储的是 /什么什么 注意“/”
    for (const auto &entry : fs::recursive_directory_iterator(dirPath))
    {
        if (fs::is_directory(entry.path()))
        {
            dirname.push_back(entry.path().string());
        }
        else if (fs::is_regular_file(entry.path()))
        {
            filename.push_back(entry.path().string());
        }
    }

    // 打开文件开写
    ofstream output(outputFileName, ios::out | ios::app | ios::binary);
    // 写入目录名长度
    int dirnameSize = dirname.size();
    output.write(reinterpret_cast<char *>(&dirnameSize), sizeof(dirnameSize));

    // 写入目录名内容
    for (const auto &dir : dirname)
    {
        int dirLength = dir.size();
        output.write(reinterpret_cast<char *>(&dirLength), sizeof(dirLength));
        output.write(dir.c_str(), dirLength);
    }

    // 写入文件名长度
    int filenameSize = filename.size();
    output.write(reinterpret_cast<char *>(&filenameSize), sizeof(filenameSize));

    // 写入文件名内容
    for (const auto &file : filename)
    {
        int fileLength = file.size();
        output.write(reinterpret_cast<char *>(&fileLength), sizeof(fileLength));
        output.write(file.c_str(), fileLength);
    }
    output.close();

    // 记录每个压缩文件的大小
    long long filesize[filenameSize];
    // 挨个压缩文件
    int i = 0;
    for (const auto &file : filename)
    {
        long long size = compressFile(file, outputFileName);
        filesize[i++] = size;
    }

    // 写压缩后的大小,文本文件形式打开
    ofstream output_1(outputFileName, ios::out | ios::app);
    output_1 << "\n";
    for (int i = 0; i < filenameSize; i++)
    {
        output_1 << filesize[i] << " ";
    }
}
// 判断是否是文件夹
bool Features::isDirectory(const string &path)
{
    return fs::is_directory(path);
}
// 解压缩文件
void Features::decompress(const string &filename, string &outputFileName)
{
    FileIO fileIO;
    fileIO.decompressFile(filename, outputFileName, fs::file_size(filename), 0);
}

void Features::decompressDir(const string &filename)
{
    ifstream inputFile(filename, ios::in | ios::binary);
    int dirnameSize, filenameSize;
    string path;

    // 读取目录名长度
    inputFile.read(reinterpret_cast<char *>(&dirnameSize), sizeof(dirnameSize));

    // 读取目录名内容
    for (int i = 0; i < dirnameSize; i++)
    {
        int pathLength;
        inputFile.read(reinterpret_cast<char *>(&pathLength), sizeof(pathLength));
        path.resize(pathLength);
        inputFile.read(&path[0], pathLength);
        fs::create_directories(path); // 创建目录
    }

    // 读取文件名长度
    inputFile.read(reinterpret_cast<char *>(&filenameSize), sizeof(filenameSize));

    // 读取文件名内容
    vector<string> filepath;
    filepath.reserve(filenameSize);
    for (int i = 0; i < filenameSize; ++i)
    {
        int fileLength;
        inputFile.read(reinterpret_cast<char *>(&fileLength), sizeof(fileLength));
        path.resize(fileLength);
        inputFile.read(&path[0], fileLength);
        filepath.push_back(path);
    }
    streampos startIndex = inputFile.tellg();
    inputFile.close();
    // 获得每个文件的压缩文件的大小
    long long *filesize = getCompressDirSize(filename, filenameSize);
    // 对各个文件进行解压
    for (int i = 0; i < filenameSize; i++)
    {
        FileIO fileIO;
        startIndex = fileIO.decompressFile(filename, filepath[i], filesize[i], startIndex);
    }
}