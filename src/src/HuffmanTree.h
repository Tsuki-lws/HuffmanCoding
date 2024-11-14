#ifndef HAFFMAN_H
#define HAFFMAN_H

#include<queue>
#include<map>
#include<string>
using namespace std;

// 带权节点
class HuffmanNode {
    public:
        char data;
        long long freq;
        HuffmanNode* left;
        HuffmanNode* right;

        // 两个构造函数
        HuffmanNode(char data, long long freq) 
            : data(data), freq(freq), left(nullptr), right(nullptr) {}
        HuffmanNode(char data, long long freq, HuffmanNode* left, HuffmanNode* right) 
            : data(data), freq(freq), left(left), right(right) {}
        
        // 优先队列是大顶堆,我们要实现小顶堆,权值从小到大排列,便于构建哈夫曼树
        struct Compare {
            //返回true时,说明a的优先级低于b
            bool operator()(HuffmanNode* a, HuffmanNode* b) {
                return a->freq > b->freq;  
            }
        };
};

class HuffmanTree {
    public:
        //从小到大排列的优先队列
        priority_queue<HuffmanNode*, vector<HuffmanNode*>, HuffmanNode::Compare> q;
        //构造函数
        HuffmanTree(map<char, long long> charFreq) {
            for (auto& p : charFreq) {
                q.push(new HuffmanNode(p.first, p.second));
            }
        }
        //创建哈夫曼树
        void createHuffmanTree();
        //获取哈夫曼编码
        map<char, string> createHuffmanCode();
        //判断是否为叶子节点
        bool isLeaf(HuffmanNode* node) {
            return node->left == nullptr && node->right == nullptr;
        }
        //返回哈夫曼树根节点
        HuffmanNode* getHuffmanRoot();
    private:
        //递归获取哈夫曼编码,存放到map中
        void subCreateHuffmanCode(HuffmanNode* root, string huffmancode, map<char, string>& haffmanCode);
};
#endif