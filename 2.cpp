#include <thread>
#include <vector>
#include <mutex>
// ...existing code...

std::mutex mtx;
std::vector<std::string> processedData(times + 1);

void processChunk(int chunkIndex, std::ifstream &inputFile, const std::vector<std::string> &charCodeArray) {
    char inputBuffer[BUFFER_SIZE];
    char outputBuffer[BUFFER_SIZE];
    int outputIndex = 0;
    unsigned char bits = 0;
    int bitcount = 0;

    inputFile.seekg(chunkIndex * BUFFER_SIZE, std::ios::beg);
    inputFile.read(inputBuffer, BUFFER_SIZE * sizeof(char));

    std::string result;
    for(size_t i = 0; i < BUFFER_SIZE; i++){
        string currentChar = charCodeArray[(unsigned char)inputBuffer[i]];
        int length = currentChar.length();
        for(size_t j = 0; j < length; j++){
            bits <<= 1;
            bits |= (currentChar[j] == '1'); 
            bitcount++;
            if(bitcount == 8){
                result += bits;
                bits = 0;
                bitcount = 0;
            }
        }
    }
    if (bitcount > 0) {
        for(int i = bitcount; i < 8; i++){
            bits <<= 1;
            bits |= 0;
        }
        result += bits;
    }

    std::lock_guard<std::mutex> lock(mtx);
    processedData[chunkIndex] = result;
}

int main() {
    // ...existing code...
    std::ifstream inputFile(filename, std::ios::binary);
    std::ofstream outputFile(outputFilename, std::ios::binary);

    std::vector<std::thread> threads;
    for(int i = 0; i < times; i++){
        threads.emplace_back(processChunk, i, std::ref(inputFile), std::ref(charCodeArray));
    }

    for(auto &t : threads) {
        t.join();
    }

    // 处理不满BUFFER_SIZE的部分
    char inputBuffer[BUFFER_SIZE];
    char outputBuffer[BUFFER_SIZE];
    int outputIndex = 0;
    unsigned char bits = 0;
    int bitcount = 0;
    long long others = filesize % BUFFER_SIZE;
    inputFile.seekg(times * BUFFER_SIZE, std::ios::beg);
    inputFile.read(inputBuffer, others * sizeof(char));

    std::string result;
    for(size_t i = 0; i < others; i++){
        string currentChar = charCodeArray[(unsigned char)inputBuffer[i]];
        int length = currentChar.length();
        for(size_t j = 0; j < length; j++){
            bits <<= 1;
            bits |= (currentChar[j] == '1'); 
            bitcount++;
            if(bitcount == 8){
                result += bits;
                bits = 0;
                bitcount = 0;
            }
        }
    }
    if (bitcount > 0) {
        for(int i = bitcount; i < 8; i++){
            bits <<= 1;
            bits |= 0;
        }
        result += bits;
    }
    processedData[times] = result;

    for(const auto &data : processedData) {
        outputFile.write(data.c_str(), data.size());
    }

    inputFile.close();
    outputFile.close();
    return 0;
}