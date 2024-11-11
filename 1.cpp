// #include <iostream>
// #include <vector>
// #include <string>
// #include <filesystem>

// using namespace std;
// namespace fs = std::filesystem;

// int main() {
//     string path = "D:\\学习\\大一上\\周易"; // 替换为实际路径

//     ostream& out = cout;
//     vector<string> fileName;
//     vector<string> dirName;
//     filesystem::path p(path);
    
//     // 把当前文件夹名字记录进去
//     dirName.push_back(path);
    
//     // 遍历这个dirPath文件夹,将文件加入到filename，将文件夹加入到dirname,相对路径了
//     for (const auto& entry : fs::recursive_directory_iterator(path)) {
//         if (fs::is_directory(entry.path())) {
//             dirName.push_back(entry.path().string().substr(path.length()));
//         } else if (fs::is_regular_file(entry.path())) {
//             fileName.push_back(entry.path().string().substr(path.length()));
//         }
//     }

//     // 输出文件和目录名（可选）
//     out << "Directories:\n";
//     for (const auto& dir : dirName) {
//         out << dir << "\n";
//     }
//     out << "Files:\n";
//     for (const auto& file : fileName) {
//         out << file << "\n";
//     }

//     return 0;
// }


#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

void output_structure(const std::string& path, const std::string& output_file) {
    std::ofstream ofs(output_file);
    if (!ofs) {
        std::cerr << "无法打开输出文件" << std::endl;
        return;
    }
    ofs << path << std::endl;
    try {
        for (const auto& entry : fs::recursive_directory_iterator(path)) {
            ofs << entry.path().string() << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "文件系统错误: " << e.what() << std::endl;
    }
}

int main() {
    std::string path = "D:\\杂乱"; // 替换为你的路径
    std::string output_file = "1.txt"; // 替换为输出文件路径

    output_structure(path, output_file);

    return 0;
}
