#include "features.h"
#include "fileIO.h"
#include "Utils.h"
// 压缩
void Features::compress(const string& filename, const string& outputFileName) {
    // 判断是文件还是文件夹
    if (isDirectory(filename)) {
        // 是文件夹
        compressDirectory(filename, outputFileName);
    } else {
        // 是文件
        compressFile(filename, outputFileName);
    }
}

long long Features::compressFile(const string& filename, const string& outputFileName) {
    long long size;
    //判断是否存在目标文件
    fs::path str(outputFileName);
    fs::directory_entry entry(str);
    //如果无目标文件,则其大小就是0
    if(entry.status().type() == fs::file_type::not_found)
        size = 0;
    else
        size = file_size(str);
    FileIO fileIO;
    map<char, long long> charFreq = fileIO.makeCharFreq(filename);
    fileIO.compressFile(filename, outputFileName);
    // 返回压缩单个文件的大小
    return file_size(str);
}
// 压缩文件夹
void Features::compressDirectory(const string& dirPath, const string& outputFileName) {    
    // 获取目录下的文件夹信息以及文件信息
    vector<string> dirname;
    vector<string> filename;

    // 将原文件夹路径完整的记录下来
    dirname.push_back(dirPath);
    // 遍历这个dirPath文件夹,将文件加入到filename，将文件夹加入到dirname,相对路径了
    // 这里存储的是 /什么什么 注意“/”
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (fs::is_directory(entry.path())) {
            dirname.push_back(entry.path().string());
        } else if (fs::is_regular_file(entry.path())) {
            filename.push_back(entry.path().string());
        }
    }

    // 打开文件开写
    ofstream output(outputFileName, ios::out | ios::app);
    int filenameSize = filename.size();
    int dirnameSize = dirname.size();
    output << dirnameSize << endl;
    for (int i = 0; i < dirnameSize; i++){
        output << dirname[i] << endl;
    }
    output << filenameSize << endl;
    for (int i = 0; i < filenameSize;i++){
        //写文件名
        output << filename[i] << endl;
    }
    output.close();

    // 记录每个压缩文件的大小
    long long filesize[filenameSize];
    // 挨个压缩文件
    for (const auto& file : filename) {
        int i = 0;
        long long size = compressFile(file, outputFileName);
        filesize[i++] = size;
    }

    // 写压缩后的大小,文本文件形式打开
    ofstream output_1(outputFileName, ios::out | ios::app);
    output_1 << "\n";
    for (int i = 0; i < filenameSize;i++){
        output_1 << filesize[i] << " " ;
    }
}
// 判断是否是文件夹
bool Features::isDirectory(const string& path) {
    return fs::is_directory(path);
}
// 解压缩文件
void Features::decompress(const string& filename, string& outputFileName) {
    FileIO fileIO;
    fileIO.decompressFile(filename,outputFileName,fs::file_size(filename),0);
}
void Features::decompressDir(const string& filename){
    ifstream inputFile(filename, ios::in);
    string dirNum, fileNum;
    int dirnameSize,filenameSize;
    string path;
    // 处理文件夹
    getline(inputFile,dirNum);
    dirnameSize = stoi(dirNum);
    for(int i = 0; i < dirnameSize; i++){
        getline(inputFile,path);
        fs::create_directories(path);
    }
    // 处理文件
    getline(inputFile,fileNum);
    filenameSize = stoi(fileNum);

    if(filenameSize == 0) {
        return;
    }
    string filepath[filenameSize];
    for(int i = 0; i < filenameSize; i++){
        getline(inputFile,path);
        filepath[i] = path;
    }
    streampos startIndex = inputFile.tellg();
    inputFile.close();
    // 获得每个文件的压缩文件的大小
    long long* filesize = getCompressDirSize(filename,filenameSize);
    // 对各个文件进行解压
    for(int i = 0; i < filenameSize; i++) {
        FileIO fileIO;
        startIndex = fileIO.decompressFile(filename,filepath[i],filesize[i],startIndex);
    }
}