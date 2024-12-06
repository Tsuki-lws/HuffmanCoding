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

// 处理移位
void FileIO::gresson(char &bits, int &bitcount, int& outputIndex,char* buffer,ofstream& file,bool data){
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

// 前序遍历存哈夫曼树到缓冲区
void FileIO::writeTreeToBuffer(ofstream& file, HuffmanNode* root, char* buffer, int& outputIndex, 
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
void FileIO::writeHuffmanTree(ofstream& file, HuffmanNode* root) {
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
void FileIO::handleEmptyFile(const string &filename, const string &outputFileName,
                             const string &prefix, ifstream &inputFile, ofstream &outputFile)
{
    fileHead filehead;
    filehead.originBytes = 0;
    filehead.alphaVarity = 0;
    outputFile.write((char *)(&filehead), sizeof(filehead));
}

// 处理非空文件非主内容信息
string *FileIO::handleNonEmptyFileHead(const string &filename,const string &outputFileName, 
                                            const string &prefix, ifstream &inputFile, ofstream &outputFile)
{
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

    outputFile.write((char *)(&filehead), sizeof(filehead));

    // clock_t a1 = clock();
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
    //  clock_t end = clock();
    // cout << "压缩处理总"<< end - a1 << endl;
    return charCodeArray;
}
// 压缩块
vector<char> FileIO::compressBlock(const char *inputBuffer, int size, const string *charCodeArray)
{
    vector<char> outputBuffer;
    unsigned char bits = 0;
    int bitcount = 0;
    for (int i = 0; i < size; i++)
    {
        string currentChar = charCodeArray[(unsigned char)inputBuffer[i]];
        int length = currentChar.length();
        for (int j = 0; j < length; j++)
        {
            bits <<= 1;
            bits |= (currentChar[j] == '1');
            bitcount++;
            if (bitcount == 8)
            {
                outputBuffer.push_back(bits);
                bits = 0;
                bitcount = 0;
            }
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
// 压缩单个文件
void FileIO::compressFile(const string &filename, const string &outputFileName, const string &prefix)
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
        if(filesize < FILE_SIZE){
            char check = '1';
            outputFile.write(&check, sizeof(check));
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
        }else{
            char check = '2';
            outputFile.write(&check, sizeof(check));
            int totalBlocks = (filesize + BUFFER_SIZE - 1) / BUFFER_SIZE;
            int maxThreads = min((int)(thread::hardware_concurrency()), totalBlocks);
            int times = (totalBlocks + maxThreads - 1) / maxThreads;
            int lastBlocksSize = filesize % BUFFER_SIZE;
            // 将总块数写入文件
            outputFile.write((char *)(&totalBlocks), sizeof(totalBlocks));
            // 将最后一块的大小写入文件
            outputFile.write((char *)(&lastBlocksSize),sizeof(lastBlocksSize));

            vector<vector<char>> inputBuffer(maxThreads, vector<char>(BUFFER_SIZE));
            vector<vector<char>> outputBuffer(maxThreads);
            vector<thread> threads;

            for (int i = 0; i < times; i++)
            {
                int currentThreads = min(maxThreads, totalBlocks - i * maxThreads);
                threads.clear();   
                for (int j = 0; j < currentThreads; j++)
                {
                    int readSize = (i == times - 1 && j == currentThreads - 1) ? lastBlocksSize : BUFFER_SIZE;
                    inputFile.read(inputBuffer[j].data(), readSize);
                    threads.emplace_back([&, j, readSize]()
                    {
                        outputBuffer[j] = compressBlock(inputBuffer[j].data(), readSize, charCodeArray);
                    });
                }
                for (auto &t : threads)
                {
                    t.join();
                }
                for (int j = 0; j < currentThreads; j++)
                {
                    // 先写入每个outputBuffer的长度
                    int bufferSize = outputBuffer[j].size();
                    outputFile.write(reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize));
                    // 再写入数据
                    outputFile.write(outputBuffer[j].data(), bufferSize);
                }
            }
        }
        inputFile.close();
        outputFile.close();
        
    }
}

// 读取压缩文件头信息
pair<fileHead, streampos> FileIO::readFileHead(const string &filename, const streampos &startIndex)
{
    fileHead filehead;
    ifstream inputFile(filename, ios::in | ios::binary);
    inputFile.seekg(startIndex);
    inputFile.read((char *)(&filehead), sizeof(filehead));

    // 记录当前文件指针位置
    streampos currentPos = inputFile.tellg();
    inputFile.close();

    return make_pair(filehead, currentPos);
}

// 读取压缩文件字符频度信息,构建哈夫曼树
pair<map<char, long long>, streampos> FileIO::readCompressTFileFreq(const string &filename,
                                                                    int alphaVarity, streampos currentPos)
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

    return make_pair(freqTable, newPos);
}


// 读取并恢复字节数据
bool FileIO::readGresson(char &bits, int &bitcount, int& inputIndex, vector<char> &buffer, 
                                            long &remainingSize, ifstream& file, bool &data) {
    if (bitcount == 0) { 
        if (inputIndex >= remainingSize) {  // 如果当前缓冲区已读完
            return false;  // 不再继续读取
        }
        bits = buffer[inputIndex++];
        bitcount = 8;
        // remainingSize--;
    }

    data = (bits >> 7) & 1;  // 取出当前位的值
    bits <<= 1;  // 移位
    bitcount--;
    return true;
}

// 从文件中读取哈夫曼树
HuffmanNode* FileIO::readTreeFromBuffer(ifstream& file, vector<char> &buffer, int& inputIndex, 
                                                        int &bitcount, char &bits, long &remainingSize) {
    bool data;
    if (!readGresson(bits, bitcount, inputIndex, buffer, remainingSize, file, data)) return nullptr;

    if (data == 1) {  // 叶子节点
        char c = 0;
        for (int i = 0; i < 8; i++) {
            if (!readGresson(bits, bitcount, inputIndex, buffer, remainingSize, file, data)) return nullptr;
            c = (c << 1) | data;  // 组装字符
        }
        return new HuffmanNode(c);  // 创建叶子节点
    } else {  // 非叶子节点
        if(inputIndex == remainingSize - 1){
            return nullptr;
        }
        HuffmanNode* node = new HuffmanNode();
        node->left = readTreeFromBuffer(file, buffer, inputIndex, bitcount, bits, remainingSize);  // 递归读取左子树
        node->right = readTreeFromBuffer(file, buffer, inputIndex, bitcount, bits, remainingSize);  // 递归读取右子树
        return node;
    }
}


// 读取哈夫曼树并返回根节点
HuffmanNode* FileIO::readHuffmanTree(ifstream& file, long size) {
    vector<char> buffer(size);  // 缓冲区
    int inputIndex = 0;  // 缓冲区当前位置
    int bitcount = 0;
    char bits = 0;
    long remainingSize = size;  // 剩余读取的字节数
    file.read((char*)(buffer.data()), size);
    return readTreeFromBuffer(file, buffer, inputIndex, bitcount, bits, remainingSize);  // 开始读取树
}



// 解压缩块
vector<char> FileIO::decompressBlock(const char* inputBuffer, int size,HuffmanNode *current)
{
    vector<char> outputBuffer;
    HuffmanNode *root = current;
    for (size_t i = 0; i < size; i++)
    {
        char byte = inputBuffer[i];
        for (int j = 0; j < 8; j++)
        {
            if (!(current->left == nullptr && current->right == nullptr))
            {
                bool bit = byte & (1 << (7 - j));
                current = bit ? current->right : current->left;
            }
            if (current->left == nullptr && current->right == nullptr)
            {
                outputBuffer.push_back(current->data);
                current = root;
            }
        }
    }
    current = root;
    return outputBuffer;
}
// 解压缩文件
streampos FileIO::decompressFile(const string &filename, const string &outputFileName,
                                 long long filesize, const streampos &startIndex)
{
    // 读取头文件信息
    auto [filehead, currentPos] = readFileHead(filename, startIndex);

    if (filehead.originBytes == 0)
    {
        ofstream outputFile(outputFileName, ios::out | ios::binary);
        outputFile.close();
        return currentPos;
    }
    // // 读取字符频度信息
    // auto [freqTable, newPos] = readCompressTFileFreq(filename, filehead.alphaVarity, currentPos);

    // // 构建哈夫曼树
    // HuffmanTree tree(freqTable);
    // tree.createHuffmanTree();
    // // 返回子节点
    // HuffmanNode *root = tree.getHuffmanRoot();
    HuffmanTree tree;

    ifstream inputFile(filename, ios::in | ios::binary);
    // 始终是覆盖写入
    ofstream outputFile(outputFileName, ios::out | ios::binary);
     // 定位到存储文件的位置
    inputFile.seekg(currentPos);
    long size;
    inputFile.read(reinterpret_cast<char*>(&size), sizeof(long));  // 读取树的大小
    HuffmanNode* root = readHuffmanTree(inputFile,size);
    // // 测试代码
    // tree.printHuffmanTree(root);

    char check;
    inputFile.read(&check,sizeof(check));
    streampos newPos = currentPos + streamoff(1) + streamoff(sizeof(size));
    if(check == '1'){
        HuffmanNode *current = root;
        // 缓冲区
        char inputBuffer[BUFFER_SIZE];
        char outputBuffer[BUFFER_SIZE];
        int outputIndex = 0;
        int writeByte = 0;
        // 主题内容的字节数
        long long mainSize = filesize - (newPos - startIndex);
        int times = mainSize / BUFFER_SIZE;
        long long others = mainSize % BUFFER_SIZE;
        for (int i = 0; i < times; i++)
        {
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
                    if (tree.isLeaf(current))
                    {
                        outputBuffer[outputIndex++] = current->data;
                        writeByte++;
                        current = root;
                        if (outputIndex == BUFFER_SIZE)
                        {
                            outputFile.write(outputBuffer, outputIndex);
                            outputIndex = 0;
                        }
                        // 处理多余的字符
                        if (writeByte >= filehead.originBytes)
                        {
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
                if (tree.isLeaf(current))
                {
                    outputBuffer[outputIndex++] = current->data;
                    writeByte++;
                    current = root;
                    if (outputIndex == BUFFER_SIZE)
                    {
                        outputFile.write(outputBuffer, outputIndex);
                        outputIndex = 0;
                    }
                    // 处理多余的字符
                    if (writeByte >= filehead.originBytes)
                    {
                        goto finish;
                    }
                }
            }
        }
    finish:;
        if (outputIndex > 0)
        {
            outputFile.write(outputBuffer, outputIndex);
        }
    }
    else if(check == '2'){
        // 读取总块数
        int totalBlocks;
        inputFile.read((char *)(&totalBlocks), sizeof(totalBlocks));
        // 读取最后一块字节数
        int lastBlockSize;
        inputFile.read((char*)(&lastBlockSize),sizeof(lastBlockSize));

        int maxThreads = min((int)(thread::hardware_concurrency()), totalBlocks);
        int times = (totalBlocks + maxThreads - 1) / maxThreads;
        int lastTime = times - 1;

        vector<vector<char>> outputBuffer(maxThreads);
        vector<vector<char>> inputBuffer(maxThreads);
        vector<thread> threads;

        for(int i = 0; i < times; i++){
            int currentThreads = min(maxThreads, totalBlocks - i * maxThreads);
            int lastCurrentTheard = currentThreads - 1;
            threads.clear();
            for (int j = 0; j < currentThreads; j++)
            {
                int readSize = 0;
                inputFile.read((char*)&readSize,sizeof(readSize));
                inputBuffer[j].resize(readSize);
                inputFile.read(inputBuffer[j].data(),readSize);
                // HuffmanNode *current = root;
                threads.emplace_back([&, j, readSize]()
                {
                    outputBuffer[j] = decompressBlock(inputBuffer[j].data(), readSize, root);
                });
            }
            for (auto &t : threads)
            {
                t.join();
            }

            // 写入解压后的数据
            for (int j = 0; j < currentThreads; j++) {
                if (i == lastTime && j == lastCurrentTheard) {
                    // 最后一块特殊处理
                    outputFile.write(outputBuffer[j].data(), lastBlockSize);
                } else {
                    outputFile.write(outputBuffer[j].data(), BUFFER_SIZE);
                }
            }
        }
    }
    streampos nowPos = inputFile.tellg();
    // 关闭文件
    inputFile.close();
    outputFile.close();
    return nowPos;
}