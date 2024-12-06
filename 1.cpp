#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>

#define BUFFER_SIZE (512 * 1024)  // 512 KB

// 哈夫曼树节点定义
struct Node {
    char character;           // 叶子节点的字符
    Node* left = nullptr;     // 左子树
    Node* right = nullptr;    // 右子树
};

// 写入缓冲区
void write_buffer(std::ofstream& file, std::vector<unsigned char>& buffer, size_t& buffer_pos) {
    if (buffer_pos >= 8) {
        size_t byte_count = buffer_pos / 8;  // 每8位为一个字节
        file.write(reinterpret_cast<char*>(buffer.data()), byte_count);  // 写入缓冲区数据
        buffer_pos %= 8;  // 更新缓冲区剩余位数
        // 将剩余的位数移至缓冲区的高位
        if (buffer_pos > 0) {
            buffer[0] &= (0xFF << (8 - buffer_pos));  // 补齐高位
        }
    }
}

// 写入位到缓冲区
void write_bit_to_buffer(std::ofstream& file, std::vector<unsigned char>& buffer, size_t& buffer_pos, unsigned char bit) {
    if (buffer_pos == 8) {
        write_buffer(file, buffer, buffer_pos);  // 缓冲区满，写入文件
    }

    size_t byte_pos = buffer_pos / 8;
    if (byte_pos >= buffer.size()) {
        buffer.push_back(0);  // 扩展缓冲区
    }
    buffer[byte_pos] |= (bit << (7 - (buffer_pos % 8)));  // 写入bit
    buffer_pos++;
}

// 写入字节到缓冲区
void write_byte_to_buffer(std::ofstream& file, unsigned char byte, std::vector<unsigned char>& buffer, size_t& buffer_pos) {
    for (int i = 7; i >= 0; --i) {
        write_bit_to_buffer(file, buffer, buffer_pos, (byte >> i) & 1);
    }
}

// 前序遍历写入哈夫曼树到缓冲区
long write_tree_to_buffer(std::ofstream& file, Node* root, std::vector<unsigned char>& buffer, size_t& buffer_pos) {
    if (!root) return 0;

    long size = 0;
    if (!root->left && !root->right) {  // 叶子节点
        write_bit_to_buffer(file, buffer, buffer_pos, 1);  // 写入标记位，1表示叶子节点
        write_byte_to_buffer(file, root->character, buffer, buffer_pos);  // 写入字符
        size += 9;  // 1位 + 1字节字符
    } else {  // 非叶子节点
        write_bit_to_buffer(file, buffer, buffer_pos, 0);  // 写入标记位，0表示非叶子节点
        size += 1;  // 1位
    }

    // 递归写入左右子树
    size += write_tree_to_buffer(file, root->left, buffer, buffer_pos);
    size += write_tree_to_buffer(file, root->right, buffer, buffer_pos);

    return size;
}

// 主函数：写入哈夫曼树结构并记录文件大小
void write_huffman_tree(std::ofstream& file, Node* root) {
    std::vector<unsigned char> buffer(BUFFER_SIZE);  // 缓冲区
    size_t buffer_pos = 0;  // 缓冲区当前位置

    // 计算树的大小并写入缓冲区
    long tree_size = write_tree_to_buffer(file, root, buffer, buffer_pos);

    // 写入文件头，记录文件大小（4字节，表示树结构部分的字节数）
    file.seekp(0, std::ios::beg);  // 定位到文件开头
    file.write(reinterpret_cast<char*>(&tree_size), sizeof(long));  // 写入文件大小

    // 递归结束后，检查缓冲区是否有数据未写入
    write_buffer(file, buffer, buffer_pos);  // 确保所有数据写入文件

    // 如果缓冲区中还有未满8位的数据，补齐并写入文件
    if (buffer_pos > 0) {
        buffer[0] &= (0xFF << (8 - buffer_pos));  // 补齐0
        write_buffer(file, buffer, buffer_pos);
    }
}
