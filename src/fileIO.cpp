#include "fileIO.h"
#include "HuffmanTree.h"
#include<bitset>

// 读取单个文件内容并构建字符频率表
map<char, long long> readFile(const string& filename) {
    ifstream file(filename, ios::in | ios::binary);
    map<char, long long> freqTable;
    vector<char> buffer(1024);
    while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
        for (size_t i = 0; i < file.gcount(); i++) {
            freqTable[buffer[i]]++;
        }
    }
    file.close();
    return freqTable;
}

// 压缩文件
void compressFile(const string& inputFilename, const string& outputFilename) {
    // 读取文件并构建字符频率表，在后面可以改到前一层，把这5行作为参数传进来
    //这样应该可以解决解压缩文件代码中的错误（好像不太行，这是独立的）
    map<char, long long> freqTable = readFile(inputFilename);
    HuffmanTree tree(freqTable);
    tree.createHuffmanTree();
    map<char, string> charCode;
    charCode = tree.createHuffmanCode();

    // 写入压缩文件
    ifstream inputFile(inputFilename, ios::in | ios::binary);
    ofstream outputFile(outputFilename, ios::out | ios::binary);
    vector<char> inputBuffer(1024);
    vector<char> outputBuffer;
    string buffer;
    while (inputFile.read(inputBuffer.data(), inputBuffer.size()) || inputFile.gcount() > 0) {
        for (size_t i = 0; i < inputFile.gcount(); i++) {
            // 获得对应的哈夫曼编码
            buffer += charCode[inputBuffer[i]];
        }
        // 每次读取一部分后进行写入
        while (buffer.size() >= 8) {
            bitset<8> bits(buffer.substr(0, 8));
            outputBuffer.push_back(static_cast<unsigned char>(bits.to_ulong()));
            buffer.erase(0, 8);
        }
        // 将缓存数组中的数据写入文件
        outputFile.write(outputBuffer.data(), outputBuffer.size());
        // 重置写入缓存数组
        outputBuffer.clear(); 
    }
    // 将最后不足8位bit的进行处理
    if (!buffer.empty()) {
        bitset<8> bits(buffer);
        outputBuffer.push_back(static_cast<unsigned char>(bits.to_ulong()));
    }
    // 将缓存数组中的数据写入文件
    outputFile.write(outputBuffer.data(), outputBuffer.size());
    // 重置写入缓存数组
    outputBuffer.clear(); 
    // 关闭文件读取和写入
    inputFile.close();
    outputFile.close();
}

// 解压缩文件
void decompressFile(const string& inputFilename, const string& outputFilename) {
    // 读取压缩文件并重建哈夫曼树
    ifstream inputFile(inputFilename, ios::in | ios::binary);
    ofstream outputFile(outputFilename, ios::out);
    HuffmanNode* root = q.top(); // 假设哈夫曼树根节点已存在
    HuffmanNode* current = root;
    char byte;
    while (inputFile.get(byte)) {
        bitset<8> bits(static_cast<unsigned char>(byte));
        for (int i = 0; i < 8; ++i) {
            if (bits[i]) {
                current = current->right;
            } else {
                current = current->left;
            }
            if (!current->left && !current->right) {
                outputFile.put(current->data);
                current = root;
            }
        }
    }
    inputFile.close();
    outputFile.close();
}
