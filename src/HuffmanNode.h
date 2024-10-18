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
