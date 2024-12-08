#include "compress.h"
#include "fileIO_C.h"
#include "Utils.h"
// 压缩
void Compress::compress(const string &filename, const string &outputFileName, const string &password)
{
    ofstream output(outputFileName, ios::out | ios::binary);
    if(!password.empty()){
        int passLength = password.size();
        output.write(reinterpret_cast<char *>(&passLength), sizeof(passLength));
        output.write(password.c_str(), passLength);
    }else{
        int passLength = 0;
        output.write((char *)(&passLength), sizeof(passLength));
    }
    // 判断是文件还是文件夹
    if (isDirectory(filename))
    {
        // 0代表是文件夹
        char zero = 0;
        output.write(&zero, sizeof(zero));
        output.close();
        // 压缩文件夹
        compressDirectory(filename, outputFileName);
    }
    else
    {
        // 1代表是文件
        char one = 1;
        output.write(&one,sizeof(one));
        // 将相对路径压入文件
        string path = fs::path(filename).filename().string();
        int pathLength = path.size();
        output.write((char *)(&pathLength), sizeof(pathLength));
        output.write(path.c_str(), pathLength);
        output.close();

        string prefix = "";
        // 压缩文件
        compressFile(filename, outputFileName,prefix);
    }
}
// 压缩单个文件
long long Compress::compressFile(const string &filename, const string &outputFileName, const string &prefix)
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
    FileIO_C fileIO;
    fileIO.compressFile(filename, outputFileName, prefix);
    return (file_size(str) - size);
}
// 压缩文件夹
void Compress::compressDirectory(const string &dirPath, const string &outputFileName)
{
    // 获取目录下的文件夹信息以及文件信息
    vector<string> dirname;
    vector<string> filename;
    // 最前缀
    string prefix = fs::path(dirPath).parent_path().string();
    int sLength = prefix.length() + 1;
    // 对于形如"D:\test"特殊处理
    if(sLength == 4){
        sLength = 3;
    }
    string first = fs::path(dirPath).filename().string();
    dirname.push_back(first);
    // 遍历这个dirPath文件夹,将文件加入到filename，将文件夹加入到dirname,相对路径了
    for (const auto &entry : fs::recursive_directory_iterator(dirPath))
    {
        if (fs::is_directory(entry.path()))
        {
            dirname.push_back(entry.path().string().substr(sLength));
        }
        else if (fs::is_regular_file(entry.path()))
        {
            filename.push_back(entry.path().string().substr(sLength));
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
        long long size = compressFile(prefix + "\\" + file, outputFileName,prefix);
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
bool Compress::isDirectory(const string &path)
{
    return fs::is_directory(path);
}
