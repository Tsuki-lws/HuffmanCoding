#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

void restore_structure(const std::string& output_file, const std::string& target_path) {
    std::ifstream ifs(output_file);
    std::string line;

    while (std::getline(ifs, line)) {
        fs::path original_path(line);
        fs::path a = original_path.relative_path();
        fs::path target_file_path = fs::path(target_path) / original_path.relative_path();

        if (fs::is_directory(original_path)) {
            fs::create_directories(target_file_path); // 创建目录
        } else {
            // 创建空文件（根据需要写入内容）
            std::ofstream(target_file_path).close();
        }
    }
}

int main() {
    std::string output_file = "1.txt"; // 替换为你的输出文件路径
    std::string target_path = "2233"; // 替换为目标路径

    restore_structure(output_file, target_path);

    return 0;
}
