// 读取移位
void FileIO::readGresson(unsigned char &bits, int &bitcount, int& inputIndex, char* buffer, long &size, ifstream& file, bool &data) {
    if (bitcount == 0) {
        if (inputIndex >= size) {
            file.read(buffer, BUFFER_SIZE);
            size = file.gcount();
            inputIndex = 0;
        }
        bits = buffer[inputIndex++];
    }

    data = (bits >> 7) & 1;
    bits <<= 1;
    bitcount++;
}

// 从缓冲区读取哈夫曼树
HuffmanNode* FileIO::readTreeFromBuffer(ifstream& file, char* buffer, int& inputIndex, int &bitcount, unsigned char &bits, long &size) {
    bool isLeaf;
    readGresson(bits, bitcount, inputIndex, buffer, size, file, isLeaf);
    if (isLeaf) {  // 叶子节点
        char data = 0;
        for (int i = 0; i < 8; i++) {
            bool bit;
            readGresson(bits, bitcount, inputIndex, buffer, size, file, bit);
            data |= (bit << (7 - i));
        }
        return new HuffmanNode(data);  // 创建叶子节点
    } else {  // 非叶子节点
        HuffmanNode* node = new HuffmanNode();
        node->left = readTreeFromBuffer(file, buffer, inputIndex, bitcount, bits, size);  // 递归读取左子树
        node->right = readTreeFromBuffer(file, buffer, inputIndex, bitcount, bits, size);  // 递归读取右子树
        return node;
    }
}

// 读取哈夫曼树并返回树的根节点
HuffmanNode* FileIO::readHuffmanTree(ifstream& file) {
    char buffer[BUFFER_SIZE];  // 缓冲区
    int inputIndex = 0;  // 缓冲区当前位置
    int bitcount = 0;
    unsigned char bits = 0;
    long size = 0;
    
    // 获取文件中树的大小
    streampos treePos = file.tellg();
    long treeSize;
    file.read(reinterpret_cast<char*>(&treeSize), sizeof(long));  // 读取树的大小
    
    // 从文件中读取哈夫曼树结构并重建
    HuffmanNode* root = readTreeFromBuffer(file, buffer, inputIndex, bitcount, bits, size);
    return root;
}
