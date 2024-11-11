#include "fileIO.h"
#include "HuffmanTree.h"
#include <bitset>
#include <filesystem>

namespace fs = std::filesystem;

// 读取单个文件内容并构建字符频率表
map<char, long long> FileIO::makeCharFreq(const string &filename)
{
    ifstream file(filename, ios::in | ios::binary);
    map<char, long long> freqTable;
    vector<char> buffer(1024);
    while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0)
    {
        for (size_t i = 0; i < file.gcount(); i++)
        {
            freqTable[buffer[i]]++;
        }
    }
    file.close();
    return freqTable;
}

// 压缩单个文件
void FileIO::compressFile(const string &filename, const string &outputFileName)
{
    // 对于空文件，直接写入0
    if (std::filesystem::file_size(filename) == 0)
    {
        // 写入压缩文件
        ifstream inputFile(filename, ios::in | ios::binary);
        // 以追加模式写入，不知道会不会有影响
        ofstream outputFile(outputFileName, ios::out | ios::binary | ios::app);

        // 写入文件头信息
        fileHead filehead;
        filehead.originBytes = 0;
        filehead.alphaVarity = 0;
        outputFile.write(reinterpret_cast<char *>(&filehead), sizeof(filehead));

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
        map<char, string> charCode = tree.createHuffmanCode();

        // 写入压缩文件
        ifstream inputFile(filename, ios::in | ios::binary);
        // 以追加模式写入，不知道会不会有影响
        ofstream outputFile(outputFileName, ios::out | ios::binary | ios::app);

        // 写入文件头信息
        fileHead filehead;
        filehead.originBytes = inputFile.seekg(0, ios::end).tellg();
        inputFile.seekg(0, ios::beg);
        filehead.alphaVarity = charCode.size();
        outputFile.write(reinterpret_cast<char *>(&filehead), sizeof(filehead));

        // 写入字符频度信息
        for (auto &entry : freqTable)
        {
            alphaCode af(entry);
            outputFile.write(reinterpret_cast<char *>(&af), sizeof(af));
        }

        vector<char> inputBuffer(1024);
        vector<char> outputBuffer;
        string buffer;
        while (inputFile.read(inputBuffer.data(), inputBuffer.size()) || inputFile.gcount() > 0)
        {
            for (size_t i = 0; i < inputFile.gcount(); i++)
            {
                buffer += charCode[inputBuffer[i]];
            }
            while (buffer.size() >= 8)
            {
                bitset<8> bits(buffer.substr(0, 8));
                outputBuffer.push_back((char)(bits.to_ulong()));
                buffer.erase(0, 8);
            }
            outputFile.write(outputBuffer.data(), outputBuffer.size());
            outputBuffer.clear();
        }
        if (!buffer.empty())
        {
            bitset<8> bits(buffer);
            outputBuffer.push_back((char)(bits.to_ulong()));
        }
        outputFile.write(outputBuffer.data(), outputBuffer.size());
        outputBuffer.clear();
        inputFile.close();
        outputFile.close();
    }
}

// 读取压缩文件头信息
fileHead FileIO::readFileHead(const string &filename)
{
    fileHead filehead;
    ifstream inputFile(filename, ios::in | ios::binary);
    inputFile.read(reinterpret_cast<char *>(&filehead), sizeof(filehead));
    inputFile.close();
    return filehead;
}

// 读取压缩文件字符频度信息,构建哈夫曼树
map<char, long long> FileIO::readCompressTFileFreq(const string &filename, int alphaVarity)
{
    ifstream inputFile(filename, ios::in | ios::binary);
    map<char, long long> freqTable;
    for (int i = 0; i < alphaVarity; i++)
    {
        alphaCode af;
        inputFile.read(reinterpret_cast<char *>(&af), sizeof(af));
        freqTable[af.alpha] = af.freq;
    }
    return freqTable;
}
// 解压缩文件
void FileIO::decompressFile(const string &filename, const string &outputFileName)
{
    ifstream inputFile(filename, ios::in | ios::binary);
    // 这个可能也要处理
    ofstream outputFile(outputFileName, ios::out);
    // 读取头文件信息
    fileHead filehead = readFileHead(filename);
    if(filehead.alphaVarity == 0 && filehead.originBytes == 0) {
        inputFile.close();
        outputFile.close();
    }else{
        // 读取字符频度信息
        map<char, long long> freqTable = readCompressTFileFreq(filename, filehead.alphaVarity);
        
        // 构建哈夫曼树
        HuffmanTree tree(freqTable);
        tree.createHuffmanTree();
        // 返回子节点
        HuffmanNode *root = tree.getHuffmanRoot();
        HuffmanNode *current = root;

        // 定位到存储文件的位置
        inputFile.seekg(sizeof(filehead) + filehead.alphaVarity * sizeof(alphaCode), ios::beg);
        // 缓冲区
        vector<char> inputBuffer(1024);
        vector<char> outputBuffer;
        while (inputFile.read(inputBuffer.data(), inputBuffer.size()) || inputFile.gcount() > 0)
        {
            for (size_t i = 0; i < inputFile.gcount(); i++)
            {
                bitset<8> bits((char)(inputBuffer[i]));
                for (int j = 0; j < 8; ++j)
                {
                    current = bits[j] ? current->right : current->left;
                    if (!current->left && !current->right)
                    {
                        outputBuffer.push_back(current->data);
                        current = root;
                    }
                }
            }
            outputFile.write(outputBuffer.data(), outputBuffer.size());
            outputBuffer.clear();
        }
        // 关闭文件
        inputFile.close();
        outputFile.close();
    }
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