using namespace std;

// 带权节点
class HuffmanNode {
public:
    char data;
    unsigned int frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    // 两个构造函数
    HuffmanNode(char data, unsigned frequency) 
        : data(data), frequency(frequency), left(nullptr), right(nullptr) {}
    HuffmanNode(char data, unsigned frequency, HuffmanNode* left, HuffmanNode* right) 
        : data(data), frequency(frequency), left(left), right(right) {}
    
};

class HuffmanTree {
public:
    HuffmanNode* root;

    HuffmanTree() : root(nullptr) {}

    // 构建哈夫曼树的方法
    void buildTree(const unordered_map<char, int>& frequencies);
    
    // 编码和解码方法
    void encode(HuffmanNode* node, const string& str);
    std::string decode(const string& encodedString);
};
