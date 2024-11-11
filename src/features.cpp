#include "features.h"
#include "fileIO.h"


void Features::compress(const string& filename, const string& outputFileName) {
    // 判断是文件还是文件夹
    if (isDirectory(filename)) {
        // 是文件夹
        compressDirectory(filename, outputFileName);
    } else {
        // 是文件
        compressFile(filename, outputFileName);
    }
}

void Features::compressFile(const string& filename, const string& outputFileName) {
    // 检查文件路径是否存在
    try {
        if (!std::filesystem::exists(filename)) {
            std::cerr << "File does not exist: " << filename << std::endl;
            return;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return;
    }

    // 暂时没有考虑outputFileName是否已经存在，感觉应该在更上层考虑
    FileIO fileIO;
    map<char, long long> charFreq = fileIO.makeCharFreq(filename);
    fileIO.compressFile(filename, outputFileName);
}

void Features::compressDirectory(const string& dirPath, const string& outputFileName) {
    // 检查目录路径是否存在
    try {
        if (!std::filesystem::exists(dirPath)) {
            std::cerr << "Directory does not exist: " << dirPath << std::endl;
            return;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return;
    }
    
    
    
    
    // 获取目录下的文件夹信息以及文件信息
    vector<string> dirname;
    vector<string> filename;

    
    // 将原文件夹路径完整的记录下来
    dirname.push_back(dirPath);
    // 遍历这个dirPath文件夹,将文件加入到filename，将文件夹加入到dirname,相对路径了
    // 这里存储的是 /什么什么 注意“/”
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (fs::is_directory(entry.path())) {
            dirname.push_back(entry.path().string().substr(dirPath.length()));
        } else if (fs::is_regular_file(entry.path())) {
            filename.push_back(entry.path().string().substr(dirPath.length()));
        }
    }

    // 打开文件开写
    ofstream output(outputFileName, ios::out | ios::app);
    output << dirname.size() << endl;
    for (int i = 0; i < dirname.size(); i++){
        output << dirname[i] << endl;
    }
    output << filename.size() << endl;
    for (int i = 0; i < filename.size();i++){
        //写文件名
        output << filename[i] << endl;
    }
    output.close();

    // 挨个压缩文件
    for (const auto& file : filename) {
        compressFile(file, outputFileName);
    }

    // 处理文件夹（如果需要递归处理文件夹，可以在这里添加逻辑）
    for (const auto& dir : dirname) {
        // 这里可以添加处理文件夹的逻辑
    }


    
}

bool Features::isDirectory(const string& path) {
    return fs::is_directory(path);
}
//到了递归(可能不用)创建文件夹
// vector<string> Features::getFilesInDirectory(const string& dirPath) {
//     vector<string> files;
//     DIR* dir = opendir(dirPath.c_str());
//     if (dir != nullptr) {
//         struct dirent* entity;
//         while ((entity = readdir(dir)) != nullptr) {
//             if (entity->d_type == DT_REG) { // 只处理常规文件
//                 files.push_back(dirPath + "/" + entity->d_name);
//             }
//         }
//         closedir(dir);
//     }
//     return files;
// }

string Features::getFileName(const string& filePath) {
    size_t pos = filePath.find_last_of("/\\");
    if (pos == string::npos) {
        return filePath;
    } else {
        return filePath.substr(pos + 1);
    }
}

void Features::decompress(const string& filename, const string& outputFileName) {

}