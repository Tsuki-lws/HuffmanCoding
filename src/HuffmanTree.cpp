#include "HuffmanTree.h"

void HuffmanTree::createHuffmanTree() {
    while (q.size() > 1) {
        // 从二叉树森林中取权值最小的两颗二叉树,q指从小到大排列的优先节点队列
        HuffmanNode* left = q.top();
        q.pop();
        HuffmanNode* right = q.top();
        q.pop();

        // 将 left 和 right 作新节点的左右子树, 构造一个新的二叉树
        // 将新二叉树根节点的权值就是 left 和 right 两颗子树的权值之和,使用t来暂代节点
        HuffmanNode* parent = new HuffmanNode('t',left->freq + right->freq, left, right);
        // 将这个新的二叉树插入到二叉树森林中
        q.push(parent);
    }
}

map<char, string> HuffmanTree::createHuffmanCode() {
    string huffmanCode;
    map<char, string> charCode;
    createHuffmanTree();
    HuffmanNode* root = q.top();
    subCreateHuffmanCode(root,huffmanCode,charCode);
    return charCode;
}

void HuffmanTree::subCreateHuffmanCode(HuffmanNode* root, 
                string huffmancode, map<char,string>& charCode) {

    if(isLeaf(root)) {
        charCode[root->data] = huffmancode;
    }
    //在分叉点复制一份相同的编码用于右子树
    string right = huffmancode;
    //是左子树加0
    if(root->left != nullptr) {
        huffmancode += '0';
        subCreateHuffmanCode(root->left,huffmancode,charCode);
    }
    //是右子树加1
    if(root->right!=nullptr){
        huffmancode = right;
        huffmancode += '1';
        subCreateHuffmanCode(root->right,huffmancode,charCode);
    }

}

HuffmanNode* HuffmanTree::getHuffmanRoot() {
    return q.top();
}
