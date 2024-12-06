// // 获取 CPU 核心数（作为线程数的上限）
    // unsigned int numThreads = thread::hardware_concurrency();
    // numThreads = min(numThreads, static_cast<unsigned int>(filenameSize));  // 不超过文件数
    //   numThreads = 4;
    // queue<thread> threadPool;
    // // 对各个文件进行解压
    // for (int i = 0; i <  min(filenameSize,5000); i++) {
    //     if (!cover[i]) {
    //         continue;
    //     }
    // // 如果当前线程数达到上限，等待最早完成的线程
    // while (threadPool.size() >= numThreads) {
    //     // 等待最早完成的线程并移除
    //     if (threadPool.front().joinable()) {
    //         threadPool.front().join();  // 等待线程完成
    //     }
    //     threadPool.pop();
    // }
    //      // 创建并推送新线程到线程池
    // threadPool.push(thread([this, &filename, &filepath, &filesize, &startIndex, i] {
    //     // 调用解压函数
    //     decompressFileTask(filename, filepath[i], filesize[i], startIndex[i]);
    // }));
    // }
    // // 等待所有线程完成
    // while (!threadPool.empty()) {
    //     if (threadPool.front().joinable()) {
    //         threadPool.front().join();  // 确保每个线程都被正确 join
    //     }
    //     threadPool.pop();
    // }