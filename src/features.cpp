#include "features.h"
#include "fileIO.h"
#include "Utils.h"
// 压缩
void Features::compress(const string &filename, const string &outputFileName, const string &password)
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
long long Features::compressFile(const string &filename, const string &outputFileName, const string &prefix)
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
    fileIO.compressFile(filename, outputFileName, prefix);
    return (file_size(str) - size);
}
// 压缩文件夹
void Features::compressDirectory(const string &dirPath, const string &outputFileName)
{
    // 获取目录下的文件夹信息以及文件信息
    vector<string> dirname;
    vector<string> filename;
    // // 将原文件夹路径完整的记录下来
    // dirname.push_back(dirPath);
    // 最前缀
    string prefix = fs::path(dirPath).parent_path().string();/*fs::path(dirPath).filename().string();// 相对路径*/
    int sLength = prefix.length() + 1;
    dirname.push_back(fs::path(dirPath).filename().string());
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
bool Features::isDirectory(const string &path)
{
    return fs::is_directory(path);
}
// 解压缩
void Features::decompress(const string& filename, string& outputFileName, int passLength){
    ifstream input(filename, ios::in | ios::binary);
    input.seekg(sizeof(passLength) + passLength * sizeof(char));
    char choice;
    input.read(&choice,sizeof(char));
    streampos currentPos = input.tellg();
    switch(choice) {
        case 0:
        {
            try{
                // 文件夹
                decompressDir(filename,outputFileName,currentPos);
            }catch(const runtime_error& e){
                throw runtime_error(e.what());
            }
            break;
        }
        case 1:
        {
            try{
                // 文件
                decompressFile(filename, outputFileName,currentPos);
            }catch(const runtime_error& e){
                throw runtime_error(e.what());
            }
            
            break;
        }
    }
}
// 解压缩文件
void Features::decompressFile(const string &filename, string &outputFileName, streampos currentPos)
{   
    // 读取文件名,将输出路径更新
    ifstream input(filename,ios::in | ios::binary);
    input.seekg(currentPos);
    int pathLength;
    input.read((char*)(&pathLength),sizeof(pathLength));
    string pathBuffer(pathLength, '\0');
    input.read(&pathBuffer[0], pathLength);
    // 加入指定前缀,
    if(outputFileName != ""){
        fs::create_directories(outputFileName);
        outputFileName += ("\\" + pathBuffer);
    }else{
        outputFileName = pathBuffer;
    }
    
    currentPos = input.tellg();
    input.close();

    int cover = true; // 默认为覆盖
    cover = checkOutputPath(outputFileName);
    if (cover == coverStatus::EXIT){
        throw runtime_error("Decompression aborted by user.");
    }
    if(!cover){ // 如果false,跳过
        return;
    }
    FileIO fileIO;
    fileIO.decompressFile(filename, outputFileName, fs::file_size(filename), currentPos);
}
// 单个处理任务
void Features::decompressFileTask(const string& filename, string& filepath, int filesize, int startIndex) {
    FileIO fileIO;
    fileIO.decompressFile(filename, filepath, filesize, startIndex);  
}
// 解压缩文件夹
void Features::decompressDir(const string &filename, const string &prefix ,streampos currentPos)
{
    ifstream inputFile(filename, ios::in | ios::binary);
    inputFile.seekg(currentPos);
    int dirnameSize, filenameSize;
    string path;
    // 判断路径前缀
    string fullPathPrefix = prefix.empty() ? "" : prefix + "\\";

    // 读取目录名长度
    inputFile.read(reinterpret_cast<char *>(&dirnameSize), sizeof(dirnameSize));
    // 读取目录名内容
    
    for (int i = 0; i < dirnameSize; i++)
    {
        int pathLength;
        inputFile.read(reinterpret_cast<char *>(&pathLength), sizeof(pathLength));
        path.resize(pathLength);
        inputFile.read(&path[0], pathLength);
        fs::create_directories(fullPathPrefix + path); // 创建目录
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
        filepath.push_back(fullPathPrefix + path);
    }
    streampos startIndex[filenameSize];
    startIndex[0] = inputFile.tellg();
    inputFile.close();
    // 获得每个文件的压缩文件的大小
    long long *filesize = getCompressDirSize(filename, filenameSize);
    // 获得每个文件前面的文件压缩大小
    for (int i = 1; i < filenameSize; i++) {
        startIndex[i] =  startIndex[i - 1] + filesize[i - 1];
    }
    // true 为覆盖
    vector<bool> cover(filenameSize, true);
    int result = coverAll(filepath,filenameSize);
    if(result == coverStatus::OTHER){
        int status;
        // 依次询问是否替代
        for(int i = 0; i < filenameSize; i++) {
            status = checkOutputPath(filepath[i]);
            if(status == coverStatus::EXIT){
                throw runtime_error("Decompression aborted by user.");
                return;
            }
            cover[i] = status;
        }
    }
    else if(result == coverStatus::SKIP){ // 相同的全部跳过
        for(int i = 0; i < filenameSize; i++){
            cover[i] = !(fs::exists(filepath[i]));
        }
    }
    FileIO fileIO;
    
    for(int i = 0; i < filenameSize; i++){
        if (!cover[i]) {
            continue;
        }
        fileIO.decompressFile(filename,filepath[i], filesize[i], startIndex[i]);
    }
    
}