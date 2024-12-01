// #include <thread>
// #include <vector>
// #include <iostream>

// void decompressFileTask(int i, const std::string& filename, const std::string& filepath, int filesize, int startIndex) {
//     FileIO fileIO;
//     if(i > 0){
//         fileIO.decompressFile(filename, filepath, filesize, startIndex + filesize);  // 注意索引更新的逻辑
//     } else {
//         fileIO.decompressFile(filename, filepath, filesize, startIndex);
//     }
// }

// void decompressFiles(const std::string& filename, const std::vector<std::string>& filepath, 
//                      const std::vector<int>& filesize, int startIndex) {
//     int filenameSize = filepath.size();
    
//     // 获取 CPU 核心数（作为线程数的上限）
//     unsigned int numThreads = std::thread::hardware_concurrency();
//     numThreads = std::min(numThreads, static_cast<unsigned int>(filenameSize));  // 不超过文件数

//     std::vector<std::thread> threads;
//     unsigned int activeThreads = 0;

//     for (int i = 0; i < filenameSize; i++) {
//         if (activeThreads >= numThreads) {
//             // 等待一个线程完成
//             for (auto& t : threads) {
//                 if (t.joinable()) {
//                     t.join();
//                     activeThreads--;
//                     break;
//                 }
//             }
//         }
//         // 分配线程来解压文件
//         threads.push_back(std::thread(decompressFileTask, i, filename, filepath[i], filesize[i], startIndex));
//         activeThreads++;
//     }

//     // 等待所有线程完成
//     for (auto& t : threads) {
//         if (t.joinable()) {
//             t.join();
//         }
//     }
// }

// int main() {
//     // 示例使用
//     std::vector<std::string> filepath = { "file1.zip", "file2.zip", "file3.zip" };
//     std::vector<int> filesize = { 100, 200, 300 };
//     int startIndex = 0;
//     decompressFiles("example.zip", filepath, filesize, startIndex);

//     return 0;
// }
