using namespace std;

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
