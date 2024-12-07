#include "Decompress.h"
#include "fileIO.h"
#include "Utils.h"
// 解压缩
void Decompress::decompress(const string& filename, string& outputFileName, int passLength){
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
void Decompress::decompressFile(const string &filename, string &outputFileName, streampos currentPos)
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
void Decompress::decompressFileTask(const string& filename, string& filepath, int filesize, int startIndex) {
    FileIO fileIO;
    fileIO.decompressFile(filename, filepath, filesize, startIndex);  
}
// 解压缩文件夹
void Decompress::decompressDir(const string &filename, const string &prefix ,streampos currentPos)
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