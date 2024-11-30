#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstring>
#include <filesystem>
#include <queue>
#include <condition_variable>

namespace fs = std::filesystem;

const int BUFFER_SIZE = 1024;  // 根据需要调整缓冲区大小

struct ThreadData {
    std::mutex mtx;  // 确保线程间同步
    std::condition_variable cv;
    std::queue<std::vector<char>> outputQueue;  // 存储处理结果的队列
    int threadsFinished = 0;  // 记录已完成的线程数量
};

void processBlock(char* inputBuffer, int blockSize, std::vector<char>& outputBuffer, int& outputIndex, unsigned char& bits, int& bitcount, 
                  ThreadData& threadData, std::string* charCodeArray) {
    // 处理数据块
    for (int i = 0; i < blockSize; i++) {
        std::string currentChar = charCodeArray[(unsigned char)inputBuffer[i]];
        int length = currentChar.length();
        for (int j = 0; j < length; j++) {
            bits <<= 1;
            bits |= (currentChar[j] == '1');
            bitcount++;
            if (bitcount == 8) {
                outputBuffer[outputIndex++] = bits;
                bits = 0;
                bitcount = 0;
            }
            if (outputIndex == BUFFER_SIZE) {
                // 将处理后的数据存入队列
                {
                    std::lock_guard<std::mutex> lock(threadData.mtx);
                    threadData.outputQueue.push(outputBuffer);
                }
                outputIndex = 0;
            }
        }
    }
}

void workerThread(std::ifstream& inputFile, long long startPos, long long endPos, ThreadData& threadData, std::string* charCodeArray) {
    inputFile.seekg(startPos);
    char inputBuffer[BUFFER_SIZE];
    unsigned char bits = 0;
    int bitcount = 0;
    int outputIndex = 0;  // 独立化 outputIndex
    std::vector<char> outputBuffer(BUFFER_SIZE);

    long long remaining = endPos - startPos;
    while (remaining > 0) {
        int blockSize = std::min(remaining, (long long)BUFFER_SIZE);
        inputFile.read(inputBuffer, blockSize * sizeof(char));
        processBlock(inputBuffer, blockSize, outputBuffer, outputIndex, bits, bitcount, threadData, charCodeArray);
        remaining -= blockSize;
    }

    if (outputIndex > 0) {
        // 将剩余数据存入队列
        {
            std::lock_guard<std::mutex> lock(threadData.mtx);
            threadData.outputQueue.push(outputBuffer);
        }
    }

    {
        std::lock_guard<std::mutex> lock(threadData.mtx);
        threadData.threadsFinished++;
    }
    threadData.cv.notify_one();
}

void writeOutput(std::ofstream& outputFile, ThreadData& threadData) {
    while (true) {
        std::vector<char> buffer;
        {
            std::lock_guard<std::mutex> lock(threadData.mtx);
            if (!threadData.outputQueue.empty()) {
                buffer = std::move(threadData.outputQueue.front());
                threadData.outputQueue.pop();
            }
        }

        if (!buffer.empty()) {
            outputFile.write(buffer.data(), buffer.size());
        }

        // 如果所有线程都已完成，并且队列已空，则退出
        if (threadData.threadsFinished == 0 && threadData.outputQueue.empty()) {
            break;
        }
    }
}

void processFile(const std::string& inputFilename, const std::string& outputFilename, std::string* charCodeArray) {
    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "无法打开输入文件" << std::endl;
        return;
    }

    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile) {
        std::cerr << "无法打开输出文件" << std::endl;
        return;
    }

    long long filesize = fs::file_size(inputFilename);
    int numThreads = 4;  // 假设使用4个线程
    long long blockSize = filesize / numThreads;

    ThreadData threadData;
    std::vector<std::thread> threads;

    // 启动线程
    for (int i = 0; i < numThreads; ++i) {
        long long startPos = i * blockSize;
        long long endPos = (i == numThreads - 1) ? filesize : (i + 1) * blockSize;
        threads.push_back(std::thread(workerThread, std::ref(inputFile), startPos, endPos, std::ref(threadData), charCodeArray));
    }

    {
        std::unique_lock<std::mutex> lock(threadData.mtx);
        threadData.cv.wait(lock, [&]{ return threadData.threadsFinished == numThreads; });
    }

    // 写入处理后的数据
    writeOutput(outputFile, threadData);

    for (auto& t : threads) {
        t.join();
    }

    inputFile.close();
    outputFile.close();
}

int main() {
    std::string inputFilename = "input.txt";
    std::string outputFilename = "output.txt";

    std::vector<std::string> charCodeArray(256);  // 初始化 charCodeArray，根据需要填充每个字符的编码

    processFile(inputFilename, outputFilename, charCodeArray.data());

    return 0;
}
