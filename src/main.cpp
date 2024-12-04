// 程序运行处
// #include "Utils.h"

// int main(int argc, char* argv[]) {


//     int choice = getEncodeOrDecode();
//     Execution(choice);
//     return 0;
// }


// #include <iostream>
// #include <string>
// #include "Utils.h"
// void compressFile(const string& filename, const string& outputName, const string& password = "") {
//     // Implement file compression logic here
//     cout << "Compressing file: " << filename << " to " << outputName << " with password: " << password << endl;
// }

// void compressDirectory(const string& directoryName, const string& outputName, const string& password) {
//     // Implement directory compression logic here
//     cout << "Compressing directory: " << directoryName << " to " << outputName << " with password: " << password << endl;
// }

// void decompressFile(const string& filename, const string& outputName = "", const string& password = "") {
//     // Implement file decompression logic here
//     cout << "Decompressing file: " << filename << " to " << outputName << " with password: " << password << endl;
// }

#include "utils.h"
using namespace std;
void compressFile(const string& filename, const string& outputName, const string& password = "") {
    // 处理文件压缩的逻辑
    cout << "Compressing " << filename << " to " << outputName;
    if (!password.empty()) {
        cout << " with password " << password;
    }
    cout << endl;
}

void decompressFile(const string& filename, const string& outputName = "", const string& password = "") {
    // 处理文件解压缩的逻辑
    cout << "Decompressing " << filename;
    if (!outputName.empty()) {
        cout << " to " << outputName;
    }
    if (!password.empty()) {
        cout << " with password " << password;
    }
    cout << endl;
}

int main(int argc, char* argv[]) {
    start();
    return 0;
}
