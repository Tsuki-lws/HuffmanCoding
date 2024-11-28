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
    // 文件中只有ASCII字符
    long long freqArray[256] = {0};
    char buffer[BUFFER_SIZE];
    long long size = fs::file_size(filename);
    int times = size / BUFFER_SIZE;
    int others = size % BUFFER_SIZE;
    
    for(int i = 0; i < times; i++) {
        file.read(buffer, BUFFER_SIZE * sizeof(char));
        // 更新频率数组
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            // // 确保读取到的字符被正确映射到数组的索引
            freqArray[(int)buffer[i] + 128]++;
        }
    }
    // 不满BUFFER_SIZE的部分
    file.read(buffer, others * sizeof(char));
    for (int i = 0; i < others; i++)
    {
        // // 确保读取到的字符被正确映射到数组的索引
        freqArray[(int)buffer[i] + 128]++;
    }
    file.close();
    // 将数组中的频率转换为 map 返回
    map<char, long long> freqTable;
    for (int i = 0; i < 256; ++i) {
        if (freqArray[i] > 0) {
            freqTable[(char)(i) - 128] = freqArray[i];
        }
    }
    return freqTable;
}

// 处理空文件
void FileIO::handleEmptyFile(const string &filename, const string &outputFileName,
                                 const string &prefix, ifstream &inputFile,ofstream &outputFile) {
    fileHead filehead;
    filehead.originBytes = 0;
    filehead.alphaVarity = 0;
    // // 只存结尾文件,在这里不知道可不可以
    // string newName;
    // if(prefix == ""){
    //     newName = fs::path(filename).filename().string();
    // }else{
    //     newName = filename.substr(prefix.length() + 1); 
    // }
    // filehead.nameLength = newName.length();
    // filehead.name = new char[filehead.nameLength + 1];
    // strncpy(filehead.name, newName.c_str(), filehead.nameLength);
    // filehead.name[filehead.nameLength] = '\0';

    // writeFileHead(outputFile, filehead);
    outputFile.write((char*)(&filehead),sizeof(filehead));
}

// 处理非空文件非主内容信息
string* FileIO::handleNonEmptyFileHead(const string &filename, 
                    const string &outputFileName, const string &prefix, ifstream &inputFile,ofstream &outputFile) {
    // 读取文件并构建字符频率表，在后面可以改到前一层，把这4行作为参数传进来
    // 这样应该可以解决解压缩文件代码中的错误（好像不太行，这是独立的）
    // clock_t start = clock();
    map<char, long long> freqTable = makeCharFreq(filename);
    // clock_t end2 = clock();
    // cout<<"1:"<<end2 - start<<endl;

    // clock_t a3 = clock();
    HuffmanTree tree = HuffmanTree(freqTable);
    // clock_t m = clock();
    // cout << "hfm数构建" << m - a3 << endl;
    // clock_t a4 = clock();
    unordered_map<char, string> charCode = tree.createHuffmanCode();
    // clock_t end1 = clock();
    // cout << "压缩处理分"<< end1 - a4 << endl;
    fileHead filehead;
    // 修改了
    filehead.originBytes = fs::file_size(filename);
    // inputFile.seekg(0, ios::beg);
    filehead.alphaVarity = charCode.size();

    outputFile.write((char*)(&filehead),sizeof(filehead));

    
    // clock_t a1 = clock();
    // 将charCode查找优化到O(1)
    static string charCodeArray[256];
    for (const auto& pair : charCode) {
        charCodeArray[(unsigned char)pair.first] = pair.second;
    }
    
    // 写入字符频度信息
    for (auto &entry : freqTable)
    {
        alphaCode af(entry);
        outputFile.write(reinterpret_cast<char *>(&af), sizeof(af));
    }
    // clock_t end = clock();
    // cout << "压缩处理总"<< end - a1 << endl;
    return charCodeArray;
}

// 压缩单个文件
void FileIO::compressFile(const string &filename, const string &outputFileName, const string &prefix)
{
    fs::path entry(filename);
    // 写入压缩文件
    ifstream inputFile(filename, ios::in | ios::binary);
    // 以追加模式写入，不知道会不会有影响
    ofstream outputFile(outputFileName, ios::out | ios::binary | ios::app);
    // 对于空文件，直接写入0
    if (std::filesystem::file_size(entry) == 0)
    {
        handleEmptyFile(filename,outputFileName,prefix, inputFile, outputFile);
    }
    else
    {
        string *charCodeArray = handleNonEmptyFileHead(filename,outputFileName,prefix,inputFile, outputFile);
        
        // 写入主内容
        char inputBuffer[BUFFER_SIZE];
        char outputBuffer[BUFFER_SIZE];
        int outputIndex = 0;
        unsigned char bits = 0;
        int bitcount = 0;
        long long filesize = fs::file_size(filename);
        int times = filesize / BUFFER_SIZE;
        for(int i = 0; i < times; i++){
            inputFile.read(inputBuffer, BUFFER_SIZE * sizeof(char));
            for(size_t i = 0; i < BUFFER_SIZE; i++){
                string currentChar = charCodeArray[(unsigned char)inputBuffer[i]];
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
        // 对于不满BUFFER_SIZE的部分处理
        long long others = filesize % BUFFER_SIZE;
        inputFile.read(inputBuffer, others * sizeof(char));
        for(size_t i = 0; i < others; i++){
            string currentChar = charCodeArray[(unsigned char)inputBuffer[i]];
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
pair<fileHead,streampos> FileIO::readFileHead(const string &filename,const streampos &startIndex)
{
    fileHead filehead;
    ifstream inputFile(filename, ios::in | ios::binary);
    inputFile.seekg(startIndex);
    // // 计算总大小并分配缓冲区
    // size_t totalSize = sizeof(filehead.alphaVarity) + sizeof(filehead.originBytes) + sizeof(filehead.nameLength) + (filehead.nameLength + 1);
    // char* buffer = new char[totalSize];
    
    // // 一次性读取所有数据
    // inputFile.read(buffer, totalSize);
    
    // // 解析缓冲区数据
    // char* ptr = buffer;
    // memcpy(&filehead.alphaVarity, ptr, sizeof(filehead.alphaVarity));
    // ptr += sizeof(filehead.alphaVarity);
    // memcpy(&filehead.originBytes, ptr, sizeof(filehead.originBytes));
    // ptr += sizeof(filehead.originBytes);
    // memcpy(&filehead.nameLength, ptr, sizeof(filehead.nameLength));
    // ptr += sizeof(filehead.nameLength);
    // filehead.name = new char[filehead.nameLength + 1];
    // memcpy(filehead.name, ptr, filehead.nameLength + 1);
    // // 释放缓冲区
    // delete[] buffer;
    inputFile.read((char*)(&filehead),sizeof(filehead));
        
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
    
    // 记录当前文件指针位置
    streampos newPos = inputFile.tellg();
    inputFile.close();
    
    return make_pair(freqTable,newPos);
}
// 解压缩文件
streampos FileIO::decompressFile(const string &filename,string &outputFileName, 
                                             long long filesize,streampos startIndex, int type)
{
    if(type == FileType::FILE_TYPE){// 是文件
        // 读取文件名,将输出路径更新
        ifstream input(filename,ios::in | ios::binary);
        input.seekg(startIndex);
        int pathLength;
        input.read((char*)(&pathLength),sizeof(pathLength));
        string pathBuffer(pathLength, '\0');
        input.read(&pathBuffer[0], pathLength);
        outputFileName = pathBuffer;
        startIndex = input.tellg();
        input.close();
    }
    // 读取头文件信息
    auto [filehead,currentPos] = readFileHead(filename,startIndex);

    // if(type == FileType::FILE_TYPE){ // 是文件
    //     // 恢复文件名,将输出路径更新
    //     string outputFilename(filehead.name, filehead.nameLength);
    //     outputFileName = outputFilename;
    // }
    
    if(filehead.originBytes == 0){
        ofstream outputFile(outputFileName, ios::out | ios::binary |ios::app);
        outputFile.close();
        return currentPos;
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
    // 主题内容的字节数
    long long mainSize = filesize - (newPos - startIndex);
    int times = mainSize / BUFFER_SIZE;
    long long others = mainSize % BUFFER_SIZE;
    for(int i = 0; i < times; i++) {
        inputFile.read(inputBuffer, BUFFER_SIZE * sizeof(char));
        for (size_t i = 0; i < BUFFER_SIZE; i++)
        {
            char byte = inputBuffer[i];
            for (int j = 0; j < 8; j++)
            {
                if (!tree.isLeaf(current))
                {
                    bool bit = byte & (1 << (7 - j));
                    current = bit ? current->right : current->left;
                }
                if(tree.isLeaf(current)){
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
    // 对于不满BUFFER_SIZE的部分
    inputFile.read(inputBuffer, others * sizeof(char));
    for (size_t i = 0; i < others; i++)
    {
        char byte = inputBuffer[i];
        for (int j = 0; j < 8; j++)
        {
            if (!tree.isLeaf(current))
            {
                bool bit = byte & (1 << (7 - j));
                current = bit ? current->right : current->left;
            }
            if(tree.isLeaf(current)){
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
finish: ;
    if(outputIndex > 0){
        outputFile.write(outputBuffer, outputIndex);
    }
    streampos nowPos = inputFile.tellg();
    // 关闭文件
    inputFile.close();
    return nowPos;
}