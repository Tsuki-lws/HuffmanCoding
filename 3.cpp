对这段代码进行修改，要求：每个线程读取对应的数据，同时对数据进行处理（你需要自己计算应该处理的块），不同线程的数据处理互不影响，最后处理完的数据严格按照for循环的顺序依次写入目标文件中
参考代码：#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;

const int BUFFER_SIZE = 1024;  // 根据需要调整缓冲区大小
std::mutex writeMutex;  // 确保每个线程写入时互斥

void processData(int startIdx, int endIdx, std::ifstream& inputFile, std::ofstream& outputFile, const std::vector<std::string>& charCodeArray) {
    char inputBuffer[BUFFER_SIZE];
    char outputBuffer[BUFFER_SIZE];
    int outputIndex = 0;
    unsigned char bits = 0;
    int bitcount = 0;

    // 移动文件指针到正确的位置
    inputFile.seekg(startIdx * BUFFER_SIZE);

    for (int i = startIdx; i < endIdx; ++i) {
        inputFile.read(inputBuffer, BUFFER_SIZE * sizeof(char));
        for (size_t i = 0; i < BUFFER_SIZE; i++) {
            std::string currentChar = charCodeArray[(unsigned char)inputBuffer[i]];
            int length = currentChar.length();
            for (size_t j = 0; j < length; j++) {
                bits <<= 1;
                bits |= (currentChar[j] == '1');
                bitcount++;
                if (bitcount == 8) {
                    outputBuffer[outputIndex++] = bits;
                    bits = 0;
                    bitcount = 0;
                }
                if (outputIndex == BUFFER_SIZE) {
                    {
                        std::lock_guard<std::mutex> lock(writeMutex);
                        outputFile.write(outputBuffer, outputIndex);
                    }
                    outputIndex = 0;
                }
            }
        }
    }

    // 写入剩余的数据
    if (outputIndex > 0) {
        std::lock_guard<std::mutex> lock(writeMutex);
        outputFile.write(outputBuffer, outputIndex);
    }
}

void processFile(const std::string& filename, std::ofstream& outputFile, const std::vector<std::string>& charCodeArray) {
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "无法打开输入文件" << std::endl;
        return;
    }

    long long filesize = fs::file_size(filename);
    int times = filesize / BUFFER_SIZE;
    int numThreads = std::thread::hardware_concurrency(); // 获取硬件支持的线程数量

    std::vector<std::thread> threads;
    int blockSize = times / numThreads;
    for (int i = 0; i < numThreads; ++i) {
        int startIdx = i * blockSize;
        int endIdx = (i == numThreads - 1) ? times : (i + 1) * blockSize;
        threads.push_back(std::thread(processData, startIdx, endIdx, std::ref(inputFile), std::ref(outputFile), std::cref(charCodeArray)));
    }

    for (auto& t : threads) {
        t.join(); // 等待所有线程完成
    }

    // 处理文件中不满BUFFER_SIZE的部分
    long long others = filesize % BUFFER_SIZE;
    if (others > 0) {
        char inputBuffer[BUFFER_SIZE];
        inputFile.read(inputBuffer, others * sizeof(char));
        unsigned char bits = 0;
        int bitcount = 0;
        int outputIndex = 0;
        for (size_t i = 0; i < others; i++) {
            std::string currentChar = charCodeArray[(unsigned char)inputBuffer[i]];
            int length = currentChar.length();
            for (size_t j = 0; j < length; j++) {
                bits <<= 1;
                bits |= (currentChar[j] == '1');
                bitcount++;
                if (bitcount == 8) {
                    outputBuffer[outputIndex++] = bits;
                    bits = 0;
                    bitcount = 0;
                }
                if (outputIndex == BUFFER_SIZE) {
                    {
                        std::lock_guard<std::mutex> lock(writeMutex);
                        outputFile.write(outputBuffer, outputIndex);
                    }
                    outputIndex = 0;
                }
            }
        }

        if (bitcount > 0) {
            // 补齐八位
            for (int i = bitcount; i < 8; i++) {
                bits <<= 1;
            }
            outputBuffer[outputIndex++] = bits;
        }

        if (outputIndex > 0) {
            std::lock_guard<std::mutex> lock(writeMutex);
            outputFile.write(outputBuffer, outputIndex);
        }
    }

    inputFile.close();
}

int main() {
    std::string filename = "input.txt";  // 输入文件
    std::ofstream outputFile("output.txt", std::ios::binary);  // 输出文件

    std::vector<std::string> charCodeArray(256); // 初始化 charCodeArray，根据需要填充每个字符的编码

    processFile(filename, outputFile, charCodeArray);

    outputFile.close();
    return 0;
}


修改代码：void FileIO::workerThread(ifstream& inputFile, long long startPos, long long endPos
                                            , ThreadData& threadData, string* charCodeArray) {
    inputFile.seekg(startPos);
    char inputBuffer[BUFFER_SIZE];
    int outputIndex = 0;
    unsigned char bits = 0;
    int bitcount = 0;
    vector<char> outputBuffer(BUFFER_SIZE);

    long long remaining = endPos - startPos;
    while (remaining > 0) {
        int blockSize = min(remaining, (long long)BUFFER_SIZE);
        inputFile.read(inputBuffer, blockSize * sizeof(char));
        // processBlock(inputBuffer, blockSize, outputIndex, bits, bitcount, outputBuffer, threadData, charCodeArray);  // 传递 charCodeArray
        try {
            processBlock(inputBuffer, blockSize, outputIndex, bits, bitcount, outputBuffer, threadData, charCodeArray);
        } catch (const std::exception& e) {
            cerr << "Exception in worker thread: " << e.what() << endl;
         throw;
        }
        remaining -= blockSize;
    }

    if (outputIndex > 0) {
        lock_guard<mutex> lock(threadData.mtx);
        threadData.outputQueue.push(outputBuffer);
        outputIndex = 0;
    }
    {
        lock_guard<mutex> lock(threadData.mtx);
        threadData.threadsFinished++;
    }
    threadData.cv.notify_one();
}


void FileIO::writeOutput(ofstream& outputFile, ThreadData& threadData) {
    while (!threadData.outputQueue.empty()) {
        auto buffer = threadData.outputQueue.front();
        outputFile.write(buffer.data(), buffer.size());
        threadData.outputQueue.pop();
    }
}

void FileIO::processFile(const string& inputFilename, const string& outputFilename, string* charCodeArray,
                                                ifstream &inputFile,ofstream &outputFile) {
    long long filesize = fs::file_size(inputFilename);
    // int numThreads = max(1u, thread::hardware_concurrency());  // 根据计算机的核心数来决定线程数
    int numThreads = 4;

    long long blockSize = filesize / numThreads;

    ThreadData threadData;

    vector<thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        long long startPos = i * blockSize;
        long long endPos = (i == numThreads - 1) ? filesize : (i + 1) * blockSize;
        threads.push_back(thread(&FileIO::workerThread, this, ref(inputFile), startPos, endPos, ref(threadData), charCodeArray));
    }

    {
        unique_lock<mutex> lock(threadData.mtx);
        threadData.cv.wait(lock, [&]{ return threadData.threadsFinished == numThreads; });
    }

    writeOutput(outputFile, threadData);

    inputFile.close();
    outputFile.close();
}