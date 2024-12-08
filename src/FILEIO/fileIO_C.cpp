#include "fileIO_C.h"
#include "HuffmanTree.h"


namespace fs = std::filesystem;
// 读取单个文件内容并构建字符频率表
map<char, long long> FileIO_C::makeCharFreq(const string &filename)
{
    ifstream file(filename, ios::in | ios::binary);
    // 使用数组来代替 map，避免多次分配内存
    // 文件中只有ASCII字符
    long long freqArray[256] = {0};
    char buffer[BUFFER_SIZE];
    long long size = fs::file_size(filename);
    int times = size / BUFFER_SIZE;
    int others = size % BUFFER_SIZE;

    for (int i = 0; i < times; i++)
    {
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
    for (int i = 0; i < 256; ++i)
    {
        if (freqArray[i] > 0)
        {
            freqTable[(char)(i)-128] = freqArray[i];
        }
    }
    return freqTable;
}

// 处理移位:哈夫曼树,正常压缩
void FileIO_C::gresson(char &bits, int &bitcount, int& outputIndex,char* buffer,ofstream& file,bool data){
    bits <<= 1;
    bits |= data;
    bitcount++;

    if(bitcount == 8){
        buffer[outputIndex++] = bits;
        bits = 0;
        bitcount = 0;
        if (outputIndex == BUFFER_SIZE)
        {
            file.write(buffer, outputIndex);
            outputIndex = 0;
        }
    }
}

// 处理移位:多线程压缩
void FileIO_C::gresson(char &bits, int &bitcount, vector<char> &buffer,bool data)
{
    bits <<= 1;
    bits |= data;
    bitcount++;

    if (bitcount == 8)
    {
        buffer.push_back(bits);
        bits = 0;
        bitcount = 0;
    }

}
// 前序遍历存哈夫曼树存到缓冲区
void FileIO_C::writeTreeToBuffer(ofstream& file, HuffmanNode* root, char* buffer, int& outputIndex, 
                                                                            int &bitcount, char &bits) {
    if (!root) return;

    if (root->left == nullptr && root->right == nullptr) {  // 叶子节点
        // 写入标记位和字符，1表示叶子节点
        gresson(bits, bitcount, outputIndex,buffer,file,1);
        char data = root->data;
        for(int i = 0; i < 8; i++){
            // 从高到底位写入
            gresson(bits, bitcount, outputIndex,buffer,file,((data >> (7 - i)) & 1));
        }
    } else {  // 非叶子节点
        // 写入标记位，0表示非叶子节点
        gresson(bits, bitcount, outputIndex,buffer,file,0);
    }

    // 递归写入左右子树
    writeTreeToBuffer(file, root->left, buffer, outputIndex,bitcount,bits);
    writeTreeToBuffer(file, root->right, buffer, outputIndex,bitcount,bits);
}
// 写入哈夫曼树结构并记录文件大小
void FileIO_C::writeHuffmanTree(ofstream& file, HuffmanNode* root) {
    char buffer[BUFFER_SIZE] = {0} ;  // 缓冲区
    int outputIndex = 0;  // 缓冲区当前位置
    int bitcount = 0;
    char bits = 0;
    // 计算树的大小并写入缓冲区
    writeTreeToBuffer(file, root, buffer, outputIndex,bitcount,bits);
    if (!!bitcount)
    {
        // 补齐八位
        for(int i = bitcount; i < 8;i++){
            bits <<= 1;
            bits |= 0;
        }
        buffer[outputIndex++] = bits;
    }
    file.write(buffer, outputIndex);
    outputIndex = 0;
}

// 处理空文件
void FileIO_C::handleEmptyFile(const string &filename, const string &outputFileName,
                             const string &prefix, ifstream &inputFile, ofstream &outputFile)
{
    fileHead filehead;
    filehead.originBytes = 0;
    filehead.alphaVarity = 0;
    outputFile.write((char *)(&filehead), sizeof(filehead));
}

// 处理非空文件非主内容信息
string *FileIO_C::handleNonEmptyFileHead(const string &filename,const string &outputFileName, 
                                            const string &prefix, ifstream &inputFile, ofstream &outputFile)
{
    // 读取文件并构建字符频率表
    map<char, long long> freqTable = makeCharFreq(filename);

    HuffmanTree tree = HuffmanTree(freqTable);
    unordered_map<char, string> charCode = tree.createHuffmanCode();
    fileHead filehead;
    // 修改了
    filehead.originBytes = fs::file_size(filename);
    filehead.alphaVarity = charCode.size();

    outputFile.write((char *)(&filehead), sizeof(filehead));

    // 将charCode查找优化到O(1)
    static string charCodeArray[256];
    for (const auto &pair : charCode)
    {
        charCodeArray[(unsigned char)pair.first] = pair.second;
    }

    HuffmanNode *root = tree.getHuffmanRoot();

    // 手动计算
    long tree_size = (10*tree.leafNumber - 1 + 7) / 8;
    outputFile.write(reinterpret_cast<char*>(&tree_size), sizeof(long));  
    writeHuffmanTree(outputFile,root);
    return charCodeArray;
}

// 压缩单个文件
void FileIO_C::compressFile(const string &filename, const string &outputFileName, const string &prefix)
{
    fs::path entry(filename);
    // 写入压缩文件
    ifstream inputFile(filename, ios::in | ios::binary);
    // 追加写入
    ofstream outputFile(outputFileName, ios::out | ios::binary | ios::app);
    // 对于空文件，直接写入0
    if (filesystem::file_size(entry) == 0)
    {
        handleEmptyFile(filename, outputFileName, prefix, inputFile, outputFile);
    }
    else
    {
        string *charCodeArray = handleNonEmptyFileHead(filename, outputFileName, prefix, inputFile, outputFile);

        // 写入主内容
        long long filesize = fs::file_size(filename);

        if (filesize < FILE_SIZE)
        {
            // 正常压缩
            compressSmallFile(inputFile, outputFile, charCodeArray, filesize);
        }
        else
        {
            // 多线程压缩
            compressLargeFile(inputFile, outputFile, charCodeArray, filesize);
        }

        inputFile.close();
        outputFile.close();
    }
}
// 处理缓冲区
void FileIO_C::processBuffer(char *inputBuffer, size_t bufferSize, string *charCodeArray, char &bits, int &bitcount, int &outputIndex, char *outputBuffer, ofstream &outputFile)
{
    for (size_t i = 0; i < bufferSize; i++)
    {
        string currentChar = charCodeArray[(unsigned char)inputBuffer[i]];
        int length = currentChar.length();
        for (size_t j = 0; j < length; j++)
        {
            gresson(bits, bitcount, outputIndex, outputBuffer, outputFile, (currentChar[j] == '1'));
        }
    }
}
// 正常压缩文件
void FileIO_C::compressSmallFile(ifstream &inputFile, ofstream &outputFile, string *charCodeArray, long long filesize)
{
    char check = '1';
    outputFile.write(&check, sizeof(check));

    char inputBuffer[BUFFER_SIZE];
    char outputBuffer[BUFFER_SIZE];
    int outputIndex = 0;
    char bits = 0;
    int bitcount = 0;
    int times = filesize / BUFFER_SIZE;

    for (int i = 0; i < times; i++)
    {
        inputFile.read(inputBuffer, BUFFER_SIZE * sizeof(char));
        processBuffer(inputBuffer, BUFFER_SIZE, charCodeArray, bits, bitcount, outputIndex, outputBuffer, outputFile);
    }
    // 对于不满BUFFER_SIZE的部分进行处理
    long long others = filesize % BUFFER_SIZE;
    inputFile.read(inputBuffer, others * sizeof(char));
    processBuffer(inputBuffer, others, charCodeArray, bits, bitcount, outputIndex, outputBuffer, outputFile);

    if (bitcount)
    {
        // 补齐八位
        for (int i = bitcount; i < 8; i++)
        {
            bits <<= 1;
            bits |= 0;
        }
        outputBuffer[outputIndex++] = bits;
    }
    outputFile.write(outputBuffer, outputIndex);
}
// 压缩块,多线程
vector<char> FileIO_C::compressBlock(const char *inputBuffer, int size, const string *charCodeArray)
{
    vector<char> outputBuffer;
    char bits = 0;
    int bitcount = 0;
    for (int i = 0; i < size; i++)
    {
        string currentChar = charCodeArray[(unsigned char)inputBuffer[i]];
        int length = currentChar.length();
        for (int j = 0; j < length; j++)
        {
            gresson(bits, bitcount, outputBuffer, (currentChar[j] == '1'));
        }
    }
    if (bitcount > 0)
    {
        for (int j = bitcount; j < 8; j++)
        {
            bits <<= 1;
            bits |= 0;
        }
        outputBuffer.push_back(bits);
    }
    return outputBuffer;
}
// 多线程压缩文件
void FileIO_C::compressLargeFile(ifstream &inputFile, ofstream &outputFile, string *charCodeArray, long long filesize)
{
    char check = '2';
    outputFile.write(&check, sizeof(check));

    int totalBlocks = (filesize + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int maxThreads = min((int)(thread::hardware_concurrency()), totalBlocks);
    int times = (totalBlocks + maxThreads - 1) / maxThreads;
    int lastBlocksSize = filesize % BLOCK_SIZE;
    // 将总块数写入文件
    outputFile.write((char *)(&totalBlocks), sizeof(totalBlocks));
    // 将最后一块的大小写入文件
    outputFile.write((char *)(&lastBlocksSize), sizeof(lastBlocksSize));

    vector<vector<char>> inputBuffer(maxThreads, vector<char>(BLOCK_SIZE));
    vector<vector<char>> outputBuffer(maxThreads);
    vector<thread> threads;
    // 多线程处理
    for (int i = 0; i < times; i++)
    {
        int currentThreads = min(maxThreads, totalBlocks - i * maxThreads);
        threads.clear();
        for (int j = 0; j < currentThreads; j++)
        {
            int readSize = (i == times - 1 && j == currentThreads - 1) ? lastBlocksSize : BLOCK_SIZE;
            inputFile.read(inputBuffer[j].data(), readSize);
            threads.emplace_back([&, j, readSize]()
                                 { outputBuffer[j] = compressBlock(inputBuffer[j].data(), readSize, charCodeArray); });
        }
        for (auto &t : threads)
        {
            t.join();
        }
        for (int j = 0; j < currentThreads; j++)
        {
            // 先写入每个outputBuffer的长度
            int bufferSize = outputBuffer[j].size();
            outputFile.write(reinterpret_cast<char *>(&bufferSize), sizeof(bufferSize));
            // 再写入数据
            outputFile.write(outputBuffer[j].data(), bufferSize);
        }
    }
}

