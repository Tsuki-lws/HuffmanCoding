#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

int main() {
    string path = "D:\\学习\\大一上\\周易"; // 替换为实际路径

    ostream& out = cout;
    vector<string> fileName;
    vector<string> dirName;
    filesystem::path p(path);
    
    // 把当前文件夹名字记录进去
    dirName.push_back(p.filename().string());
    
    // 要压缩的文件夹的前面指导根目录的字符串
    string headPath;
    if (path.find('\\') != string::npos) {
        headPath = path.substr(0, path.find(p.filename().string()));
    } else {
        headPath = "";
    }
    
    // 记录子文件(夹)
    for (auto const& entry : recursive_directory_iterator(path)) {
        if (entry.status().type() == file_type::directory) {
            dirName.push_back(entry.path().string().substr(headPath.length()));
        } else {
            fileName.push_back(entry.path().string().substr(headPath.length()));
        }
    }

    // 输出文件和目录名（可选）
    out << "Directories:\n";
    for (const auto& dir : dirName) {
        out << dir << "\n";
    }
    out << "Files:\n";
    for (const auto& file : fileName) {
        out << file << "\n";
    }

    return 0;
}
