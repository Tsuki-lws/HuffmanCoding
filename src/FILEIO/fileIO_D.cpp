#include "fileIO_D.h"
#include "HuffmanTree.h"


namespace fs = std::filesystem;
// 读取压缩文件头信息
pair<fileHead, streampos> FileIO_D::readFileHead(const string &filename, const streampos &startIndex)
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

// 读取并恢复字节数据
bool FileIO_D::readGresson(char &bits, int &bitcount, int& inputIndex, vector<char> &buffer, 
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
HuffmanNode* FileIO_D::readTreeFromBuffer(ifstream& file, vector<char> &buffer, int& inputIndex, 
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
HuffmanNode* FileIO_D::readHuffmanTree(ifstream& file, long size) {
    vector<char> buffer(size);  // 缓冲区
    int inputIndex = 0;  // 缓冲区当前位置
    int bitcount = 0;
    char bits = 0;
    long remainingSize = size;  // 剩余读取的字节数
    file.read((char*)(buffer.data()), size);
    return readTreeFromBuffer(file, buffer, inputIndex, bitcount, bits, remainingSize);  // 开始读取树
}



// 解压缩块
vector<char> FileIO_D::decompressBlock(const char* inputBuffer, int size,HuffmanNode *current)
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

// // 解压缩文件
// streampos FileIO_D::decompressFile(const string &filename, const string &outputFileName,
//                                  long long filesize, const streampos &startIndex)
// {
//     // 读取头文件信息
//     auto [filehead, currentPos] = readFileHead(filename, startIndex);

//     if (filehead.originBytes == 0)
//     {
//         ofstream outputFile(outputFileName, ios::out | ios::binary);
//         outputFile.close();
//         return currentPos;
//     }

//     // // 返回子节点
//     HuffmanTree tree;

//     ifstream inputFile(filename, ios::in | ios::binary);
//     // 始终是覆盖写入
//     ofstream outputFile(outputFileName, ios::out | ios::binary);
//      // 定位到存储文件的位置
//     inputFile.seekg(currentPos);
//     long size;
//     inputFile.read(reinterpret_cast<char*>(&size), sizeof(long));  // 读取树的大小
//     HuffmanNode* root = readHuffmanTree(inputFile,size);

//     // 检查是正常压缩还是多线程压缩
//     char check;
//     inputFile.read(&check,sizeof(check));
//     streampos newPos = currentPos + streamoff(1) + streamoff(sizeof(size));
//     if(check == '1'){
//         HuffmanNode *current = root;
//         // 缓冲区
//         char inputBuffer[BUFFER_SIZE];
//         char outputBuffer[BUFFER_SIZE];
//         int outputIndex = 0;
//         int writeByte = 0;
//         // 主题内容的字节数
//         long long mainSize = filesize - (newPos - startIndex);
//         int times = mainSize / BUFFER_SIZE;
//         long long others = mainSize % BUFFER_SIZE;
//         for (int i = 0; i < times; i++)
//         {
//             inputFile.read(inputBuffer, BUFFER_SIZE * sizeof(char));
//             for (size_t i = 0; i < BUFFER_SIZE; i++)
//             {
//                 char byte = inputBuffer[i];
//                 for (int j = 0; j < 8; j++)
//                 {
//                     if (!tree.isLeaf(current))
//                     {
//                         bool bit = byte & (1 << (7 - j));
//                         current = bit ? current->right : current->left;
//                     }
//                     if (tree.isLeaf(current))
//                     {
//                         outputBuffer[outputIndex++] = current->data;
//                         writeByte++;
//                         current = root;
//                         if (outputIndex == BUFFER_SIZE)
//                         {
//                             outputFile.write(outputBuffer, outputIndex);
//                             outputIndex = 0;
//                         }
//                         // 处理多余的字符
//                         if (writeByte >= filehead.originBytes)
//                         {
//                             goto finish;
//                         }
//                     }
//                 }
//             }
//         }
//         // 对于不满BUFFER_SIZE的部分
//         inputFile.read(inputBuffer, others * sizeof(char));
//         for (size_t i = 0; i < others; i++)
//         {
//             char byte = inputBuffer[i];
//             for (int j = 0; j < 8; j++)
//             {
//                 if (!tree.isLeaf(current))
//                 {
//                     bool bit = byte & (1 << (7 - j));
//                     current = bit ? current->right : current->left;
//                 }
//                 if (tree.isLeaf(current))
//                 {
//                     outputBuffer[outputIndex++] = current->data;
//                     writeByte++;
//                     current = root;
//                     if (outputIndex == BUFFER_SIZE)
//                     {
//                         outputFile.write(outputBuffer, outputIndex);
//                         outputIndex = 0;
//                     }
//                     // 处理多余的字符
//                     if (writeByte >= filehead.originBytes)
//                     {
//                         goto finish;
//                     }
//                 }
//             }
//         }
//     finish:;
//         if (outputIndex > 0)
//         {
//             outputFile.write(outputBuffer, outputIndex);
//         }
//     }
//     else if(check == '2'){
//         // 读取总块数
//         int totalBlocks;
//         inputFile.read((char *)(&totalBlocks), sizeof(totalBlocks));
//         // 读取最后一块字节数
//         int lastBlockSize;
//         inputFile.read((char*)(&lastBlockSize),sizeof(lastBlockSize));

//         int maxThreads = min((int)(thread::hardware_concurrency()), totalBlocks);
//         int times = (totalBlocks + maxThreads - 1) / maxThreads;
//         int lastTime = times - 1;

//         vector<vector<char>> outputBuffer(maxThreads);
//         vector<vector<char>> inputBuffer(maxThreads);
//         vector<thread> threads;

//         for(int i = 0; i < times; i++){
//             int currentThreads = min(maxThreads, totalBlocks - i * maxThreads);
//             int lastCurrentTheard = currentThreads - 1;
//             threads.clear();
//             for (int j = 0; j < currentThreads; j++)
//             {
//                 int readSize = 0;
//                 inputFile.read((char*)&readSize,sizeof(readSize));
//                 inputBuffer[j].resize(readSize);
//                 inputFile.read(inputBuffer[j].data(),readSize);
//                 threads.emplace_back([&, j, readSize]()
//                 {
//                     outputBuffer[j] = decompressBlock(inputBuffer[j].data(), readSize, root);
//                 });
//             }
//             for (auto &t : threads)
//             {
//                 t.join();
//             }

//             // 写入解压后的数据
//             for (int j = 0; j < currentThreads; j++) {
//                 if (i == lastTime && j == lastCurrentTheard) {
//                     // 最后一块特殊处理
//                     outputFile.write(outputBuffer[j].data(), lastBlockSize);
//                 } else {
//                     outputFile.write(outputBuffer[j].data(), BLOCK_SIZE);
//                 }
//             }
//         }
//     }
//     streampos nowPos = inputFile.tellg();
//     // 关闭文件
//     inputFile.close();
//     outputFile.close();
//     return nowPos;
// }

// 解压缩文件
void FileIO_D::decompressFile(const string &filename, const string &outputFileName,
                                   long long filesize, const streampos &startIndex)
{
    // 读取头文件信息
    auto [filehead, currentPos] = readFileHead(filename, startIndex);

    if (filehead.originBytes == 0)
    {
        ofstream outputFile(outputFileName, ios::out | ios::binary);
    }
    HuffmanTree tree;
    ifstream inputFile(filename, ios::in | ios::binary);
    ofstream outputFile(outputFileName, ios::out | ios::binary);
    inputFile.seekg(currentPos);

    // 读取树的大小并构建Huffman树
    long size;
    inputFile.read(reinterpret_cast<char*>(&size), sizeof(long));
    HuffmanNode* root = readHuffmanTree(inputFile, size);

    // 检查压缩方式
    char check;
    inputFile.read(&check, sizeof(check));
    streampos newPos = currentPos + streamoff(1 + sizeof(size) + size) ;

    if (check == '1')
    {
        decompressWithSingleThread(inputFile, outputFile, tree, root, filesize, newPos - startIndex, filehead.originBytes);
    //     HuffmanNode *current = root;
    //     // 缓冲区
    //     char inputBuffer[BUFFER_SIZE];
    //     char outputBuffer[BUFFER_SIZE];
    //     int outputIndex = 0;
    //     int writeByte = 0;
    //     // 主题内容的字节数
    //     long long mainSize = filesize - (newPos - startIndex);
    //     int times = mainSize / BUFFER_SIZE;
    //     long long others = mainSize % BUFFER_SIZE;
    //     for (int i = 0; i < times; i++)
    //     {
    //         inputFile.read(inputBuffer, BUFFER_SIZE * sizeof(char));
    //         for (size_t i = 0; i < BUFFER_SIZE; i++)
    //         {
    //             char byte = inputBuffer[i];
    //             for (int j = 0; j < 8; j++)
    //             {
    //                 if (!tree.isLeaf(current))
    //                 {
    //                     bool bit = byte & (1 << (7 - j));
    //                     current = bit ? current->right : current->left;
    //                 }
    //                 if (tree.isLeaf(current))
    //                 {
    //                     outputBuffer[outputIndex++] = current->data;
    //                     writeByte++;
    //                     current = root;
    //                     if (outputIndex == BUFFER_SIZE)
    //                     {
    //                         outputFile.write(outputBuffer, outputIndex);
    //                         outputIndex = 0;
    //                     }
    //                     // 处理多余的字符
    //                     if (writeByte >= filehead.originBytes)
    //                     {
    //                         goto finish;
    //                     }
    //                 }
    //             }
    //         }
    //     }
    //     // 对于不满BUFFER_SIZE的部分
    //     inputFile.read(inputBuffer, others * sizeof(char));
    //     for (size_t i = 0; i < others; i++)
    //     {
    //         char byte = inputBuffer[i];
    //         for (int j = 0; j < 8; j++)
    //         {
    //             if (!tree.isLeaf(current))
    //             {
    //                 bool bit = byte & (1 << (7 - j));
    //                 current = bit ? current->right : current->left;
    //             }
    //             if (tree.isLeaf(current))
    //             {
    //                 outputBuffer[outputIndex++] = current->data;
    //                 writeByte++;
    //                 current = root;
    //                 if (outputIndex == BUFFER_SIZE)
    //                 {
    //                     outputFile.write(outputBuffer, outputIndex);
    //                     outputIndex = 0;
    //                 }
    //                 // 处理多余的字符
    //                 if (writeByte >= filehead.originBytes)
    //                 {
    //                     goto finish;
    //                 }
    //             }
    //         }
    //     }
    // finish:;
    //     if (outputIndex > 0)
    //     {
    //         outputFile.write(outputBuffer, outputIndex);
    //     }
    }
    else if (check == '2')
    {
        decompressWithMultiThread(inputFile, outputFile, tree, root, filesize, newPos, filehead.originBytes);
    }
}

void FileIO_D::decompressWithSingleThread(ifstream &inputFile, ofstream &outputFile,
                                               HuffmanTree &tree, HuffmanNode *root,
                                               long long filesize, streampos headSize, long long originBytes)
{
    HuffmanNode* current = root;
    char inputBuffer[BUFFER_SIZE], outputBuffer[BUFFER_SIZE];
    int outputIndex = 0, writeByte = 0;
    long long mainSize = filesize - headSize;
    int times = mainSize / BUFFER_SIZE;
    long long others = mainSize % BUFFER_SIZE;
    long long size;
    for (int i = 0; i < times + 1; i++)
    {
        size = (i == times ? others : BUFFER_SIZE);
        inputFile.read(inputBuffer, size);
        for (size_t i = 0; i < size; i++)
        {
            char byte = inputBuffer[i];
            // 解码字节
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
                    if (writeByte >= originBytes)
                    {
                        goto finish;
                    }
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



void FileIO_D::decompressWithMultiThread(ifstream &inputFile, ofstream &outputFile,
                                              HuffmanTree &tree, HuffmanNode *root,
                                              long long filesize, streampos newPos, long long originBytes)
{
    int totalBlocks;
    inputFile.read(reinterpret_cast<char*>(&totalBlocks), sizeof(totalBlocks));
    int lastBlockSize;
    inputFile.read(reinterpret_cast<char*>(&lastBlockSize), sizeof(lastBlockSize));

    int maxThreads = min(static_cast<int>(thread::hardware_concurrency()), totalBlocks);
    int times = (totalBlocks + maxThreads - 1) / maxThreads;
    int lastTime = times - 1;

    vector<vector<char>> outputBuffer(maxThreads), inputBuffer(maxThreads);
    vector<thread> threads;

    for (int i = 0; i < times; ++i)
    {
        int currentThreads = min(maxThreads, totalBlocks - i * maxThreads);
        int lastCurrentThread = currentThreads - 1;

        threads.clear();
        for (int j = 0; j < currentThreads; ++j)
        {
            int readSize = 0;
            inputFile.read(reinterpret_cast<char*>(&readSize), sizeof(readSize));
            inputBuffer[j].resize(readSize);
            inputFile.read(inputBuffer[j].data(), readSize);

            threads.emplace_back([&, j, readSize] {
                outputBuffer[j] = decompressBlock(inputBuffer[j].data(), readSize, root);
            });
        }

        for (auto &t : threads)
        {
            t.join();
        }

        // 写入解压后的数据
        for (int j = 0; j < currentThreads; ++j)
        {
            if (i == lastTime && j == lastCurrentThread)
            {
                outputFile.write(outputBuffer[j].data(), lastBlockSize);
            }
            else
            {
                outputFile.write(outputBuffer[j].data(), BLOCK_SIZE);
            }
        }
    }
}
