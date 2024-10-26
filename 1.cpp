#include <fstream>
#include <iostream>

struct FileHead {
    int id;
    char name[20];
};

int main() {
    // 写入数据
    std::ofstream outputFile("output.bin", std::ios::binary);
    FileHead filehead = {1, "example"};
    outputFile.write((char*)(&filehead), sizeof(filehead));
    outputFile.close();

    // 读取数据
    std::ifstream inputFile("output.bin", std::ios::binary);
    FileHead readFilehead;
    inputFile.read((char*)(&readFilehead), sizeof(readFilehead));
    inputFile.close();

    // 输出读取的数据
    std::cout << "ID: " << readFilehead.id << std::endl;
    std::cout << "Name: " << readFilehead.name << std::endl;

    return 0;
}