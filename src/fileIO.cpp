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

void writeFileHead(ofstream &outputFile, fileHead filehead){
    // 写入结构体的非指针成员
    outputFile.write(reinterpret_cast<const char*>(&filehead.alphaVarity), sizeof(filehead.alphaVarity));
    outputFile.write(reinterpret_cast<const char*>(&filehead.originBytes), sizeof(filehead.originBytes));
    outputFile.write(reinterpret_cast<const char*>(&filehead.nameLength), sizeof(filehead.nameLength));
    // 写入文件名内容
    outputFile.write(filehead.name, filehead.nameLength + 1);
}

// 处理空文件
void FileIO::handleEmptyFile(const string &filename, const string &outputFileName,
                                 const string &prefix, ifstream &inputFile,ofstream &outputFile) {
    fileHead filehead;
    filehead.originBytes = 0;
    filehead.alphaVarity = 0;
    // 只存结尾文件,在这里不知道可不可以
    string newName;
    if(prefix == ""){
        newName = fs::path(filename).filename().string();
    }else{
        newName = filename.substr(prefix.length() + 1); 
    }
    filehead.nameLength = newName.length();
    filehead.name = new char[filehead.nameLength + 1];
    strncpy(filehead.name, newName.c_str(), filehead.nameLength);
    filehead.name[filehead.nameLength] = '\0';

    writeFileHead(outputFile, filehead);
}

// 处理非空文件头信息
pair<map<char, long long>,unordered_map<char, string>> FileIO::handleNonEmptyFileHead(const string &filename, 
                    const string &outputFileName, const string &prefix, ifstream &inputFile,ofstream &outputFile) {
    // 读取文件并构建字符频率表，在后面可以改到前一层，把这4行作为参数传进来
    // 这样应该可以解决解压缩文件代码中的错误（好像不太行，这是独立的）
    map<char, long long> freqTable = makeCharFreq(filename);
    HuffmanTree tree(freqTable);
    tree.createHuffmanTree();
    unordered_map<char, string> charCode = tree.createHuffmanCode();
    fileHead filehead;
    filehead.originBytes = inputFile.seekg(0, ios::end).tellg();
    inputFile.seekg(0, ios::beg);
    filehead.alphaVarity = charCode.size();
    // 只存结尾文件,在这里不知道可不可以
    string newName;
    if(prefix == ""){
        newName = fs::path(filename).filename().string();
    }else{
        newName = filename.substr(prefix.length() + 1); 
    }
    filehead.nameLength = newName.length();
    filehead.name = new char[filehead.nameLength + 1];
    strncpy(filehead.name, newName.c_str(), filehead.nameLength);
    filehead.name[filehead.nameLength] = '\0';
    writeFileHead(outputFile, filehead);

    return make_pair(freqTable, charCode);
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
        auto[freqTable,charCode] = handleNonEmptyFileHead(filename,outputFileName,prefix,inputFile, outputFile);
        // 将charCode查找优化到O(1)
        string charCodeArray[256];
        for (const auto& pair : charCode) {
            charCodeArray[(unsigned char)pair.first] = pair.second;
        }

        // 写入字符频度信息
        for (auto &entry : freqTable)
        {
            alphaCode af(entry);
            outputFile.write(reinterpret_cast<char *>(&af), sizeof(af));
        }

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
    inputFile.read(reinterpret_cast<char*>(&filehead.alphaVarity), sizeof(filehead.alphaVarity));
    inputFile.read(reinterpret_cast<char*>(&filehead.originBytes), sizeof(filehead.originBytes));
    inputFile.read(reinterpret_cast<char*>(&filehead.nameLength), sizeof(filehead.nameLength));
    filehead.name = new char[filehead.nameLength + 1];
    // 读取文件名内容
    inputFile.read(filehead.name, filehead.nameLength + 1);
    
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
    // 读取头文件信息
    auto [filehead,currentPos] = readFileHead(filename,startIndex);

    if(type == FileType::FILE_TYPE){ // 是文件
        // 恢复文件名,将输出路径更新
        string outputFilename(filehead.name, filehead.nameLength);
        outputFileName = outputFilename;
    }
    
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