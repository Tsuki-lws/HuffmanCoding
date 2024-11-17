#include "fileIO.h"
#include "HuffmanTree.h"
#include <bitset>
#include <filesystem>
#include <cstring>

namespace fs = std::filesystem;

map<char, long long> FileIO::makeCharFreq(const string &filename)
{
    ifstream file(filename, ios::in | ios::binary);
    
    // 使用数组来代替 map，避免多次分配内存
    // 假设文件中只有ASCII字符
    long long freqArray[256] = {0};
    
    vector<char> buffer(BUFFER_SIZE);
    
    // 批量读取数据
    while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0)
    {
        // 更新频率数组
        for (size_t i = 0; i < file.gcount(); i++)
        {
            // 确保读取到的字符被正确映射到数组的索引
            unsigned char ch = (unsigned char)(buffer[i]);  // 转换为无符号字符
            freqArray[ch]++;
        }
    }
    file.close();
    // 将数组中的频率转换为 map 返回
    map<char, long long> freqTable;
    for (int i = 0; i < 256; ++i) {
        if (freqArray[i] > 0) {
            freqTable[(char)(i)] = freqArray[i];
        }
    }
    
    return freqTable;
}


// 压缩单个文件
void FileIO::compressFile(const string &filename, const string &outputFileName)
{
    fs::path entry(filename);
    // 对于空文件，直接写入0
    if (std::filesystem::file_size(entry) == 0)
    {
        // 读取原文件
        ifstream inputFile(filename, ios::in | ios::binary);
        // 以追加模式写入，不知道会不会有影响
        ofstream outputFile(outputFileName, ios::out | ios::binary | ios::app);

        // 写入文件头信息
        fileHead filehead;
        filehead.originBytes = 0;
        filehead.alphaVarity = 0;
        filehead.nameLength = filename.length();
        strncpy(filehead.name, filename.c_str(), sizeof(filehead.name) - 1); // 将文件名存入字符数组

        outputFile.write(reinterpret_cast<char *>(&filehead), sizeof(filehead));

        outputFile.write("\n",1);

        inputFile.close();
        outputFile.close();
    }
    else
    {
        // 读取文件并构建字符频率表，在后面可以改到前一层，把这4行作为参数传进来
        // 这样应该可以解决解压缩文件代码中的错误（好像不太行，这是独立的）
        map<char, long long> freqTable = makeCharFreq(filename);
        HuffmanTree tree(freqTable);
        tree.createHuffmanTree();
        unordered_map<char, string> charCode = tree.createHuffmanCode();

        // 写入压缩文件
        ifstream inputFile(filename, ios::in | ios::binary);
        // 以追加模式写入，不知道会不会有影响
        ofstream outputFile(outputFileName, ios::out | ios::binary | ios::app);

        // 写入文件头信息
        fileHead filehead;
        filehead.originBytes = inputFile.seekg(0, ios::end).tellg();
        inputFile.seekg(0, ios::beg);
        filehead.alphaVarity = charCode.size();
        filehead.nameLength = filename.length();
        strncpy(filehead.name, filename.c_str(), sizeof(filehead.name) - 1); // 将文件名存入字符数组
        outputFile.write(reinterpret_cast<char *>(&filehead), sizeof(filehead));
        // // 换行符隔开
        // outputFile.write("\n",1);

        // 写入字符频度信息
        for (auto &entry : freqTable)
        {
            alphaCode af(entry);
            outputFile.write(reinterpret_cast<char *>(&af), sizeof(af));
        }

        // // 换行符隔开
        // outputFile.write("\n",1);

        // 写入主内容
        char inputBuffer[BUFFER_SIZE];
        char outputBuffer[BUFFER_SIZE];
        int outputIndex = 0;
        unsigned char bits = 0;
        int bitcount = 0;
        while (inputFile.read(inputBuffer, sizeof(inputBuffer)) ||  inputFile.gcount() > 0)
        {   
            int count = inputFile.gcount();
            for(size_t i = 0; i < count; i++){
                string currentChar = charCode[inputBuffer[i]];
                int length = currentChar.length();
                for(size_t j = 0; j < length; j++){
                    bits <<= 1;
                    bits |= (currentChar[j] == '1'); 
                    bitcount++;
                    if(bitcount == 8){
                        outputBuffer[outputIndex++] = bits;
                        bits = 0;
                        bitcount = 0;
                    }
                    if(outputIndex == BUFFER_SIZE){
                        outputFile.write(outputBuffer, outputIndex);
                        outputIndex = 0;
                    }
                }
            }
        }
        if (!!bitcount)
        {
            // 补齐八位
            for(int i = bitcount; i < 8;i++){
                bits <<= 1;
                bits |= 0;
            }
            outputBuffer[outputIndex++] = bits;
        }
        outputFile.write(outputBuffer, outputIndex);
        inputFile.close();
        outputFile.close();
    }
}

// 读取压缩文件头信息
pair<fileHead,streampos> FileIO::readFileHead(const string &filename)
{
    fileHead filehead;
    ifstream inputFile(filename, ios::in | ios::binary);
    inputFile.read(reinterpret_cast<char*>(&filehead), sizeof(filehead));
    // inputFile.get(); // 读取换行符
    
    // 记录当前文件指针位置
    streampos currentPos = inputFile.tellg();
    inputFile.close();
    
    return make_pair(filehead,currentPos);
}

// 读取压缩文件字符频度信息,构建哈夫曼树
pair<map<char, long long>,streampos> FileIO::readCompressTFileFreq(const string &filename,
                                                        int alphaVarity,streampos currentPos)
{
    ifstream inputFile(filename, ios::in | ios::binary);
    inputFile.seekg(currentPos);

    map<char, long long> freqTable;
    for (int i = 0; i < alphaVarity; i++)
    {
        alphaCode af;
        inputFile.read(reinterpret_cast<char *>(&af), sizeof(af));
        freqTable[af.alpha] = af.freq;
    }
    
    // inputFile.get(); // 读取换行符
    // 记录当前文件指针位置
    streampos newPos = inputFile.tellg();
    inputFile.close();
    
    return make_pair(freqTable,newPos);
}
// 解压缩文件
void FileIO::decompressFile(const string &filename, string &outputFileName)
{
    // 读取头文件信息
    auto [filehead,currentPos] = readFileHead(filename);

    // 恢复文件名,将输出路径更新
    string outputFilename(filehead.name, filehead.nameLength);
    outputFileName = outputFilename;
    if(filehead.originBytes == 0){
        ofstream outputFile(outputFileName, ios::out | ios::binary |ios::app);
        outputFile.close();
        return;
    }
    // 读取字符频度信息
    auto [freqTable, newPos] = readCompressTFileFreq(filename, filehead.alphaVarity,currentPos);
    
    // 构建哈夫曼树
    HuffmanTree tree(freqTable);
    tree.createHuffmanTree();
    // 返回子节点
    HuffmanNode *root = tree.getHuffmanRoot();
    HuffmanNode *current = root;

    ifstream inputFile(filename, ios::in | ios::binary);
    // 这个可能也要处理
    ofstream outputFile(outputFileName, ios::out | ios::binary |ios::app);
    // 定位到存储文件的位置
    inputFile.seekg(newPos);
    // 缓冲区
    char inputBuffer[BUFFER_SIZE];
    char outputBuffer[BUFFER_SIZE];
    int outputIndex = 0;
    int writeByte = 0;
    while (inputFile.read(inputBuffer, sizeof(inputBuffer)) || inputFile.gcount() > 0)
    {
        int count = inputFile.gcount();
        for (size_t i = 0; i < count; i++)
        {
            char byte = inputBuffer[i];
            for (int j = 0; j < 8; j++)
            {
                bool bit = byte & (1 << (7 - j));
                current = bit ? current->right : current->left;
                if (!current->left && !current->right)
                {
                    outputBuffer[outputIndex++] = current->data;
                    writeByte++;
                    current = root;
                    if(outputIndex == BUFFER_SIZE ){
                        outputFile.write(outputBuffer, outputIndex);
                        outputIndex = 0;
                    }
                    // 处理多余的字符
                    if(writeByte >= filehead.originBytes){
                        goto finish;
                    }
                }
            }
        }
    }
finish: ;
    if(outputIndex > 0){
        outputFile.write(outputBuffer, outputIndex);
    }
    // // 获取文件大小
    // outputFile.close(); // 关闭文件以确保所有数据都写入
    // ifstream checkFile(outputFileName, ios::in | ios::binary);
    // long long fileSize = checkFile.seekg(0, ios::end).tellg();
    // checkFile.close();
    // if(fileSize == filehead.originBytes + 1){
    //     // 使用resize_file缩减文件大小
    //     fs::resize_file(outputFileName, fileSize - 1);
    // }
    // 关闭文件
    inputFile.close();
    
}
// // 判断是否是文件夹
// bool fileIO::isDirectory(const string& filename) {
//     return fs::is_directory(filename);
// }

// // 压缩文件,输出到outputFileName
// void fileIO::compress(const string& filename, const string& outputFileName) {
//     if(isDirectory(filename)){
//         compressDirectory(filename,outputFileName);
//     }else{
//         compressFile(filename,outputFileName);
//     }
// }
// // 压缩文件夹,输出到outputFileName
// void fileIO::compressDirectory(const string& dirPath, const string& outputFileName) {
//     for (const auto& entry : fs::directory_iterator(dirPath)) {
//         if (fs::is_regular_file(entry.path())) {
//             compressFile(entry.path().string(),outputFileName);
//         }
//     }
// }