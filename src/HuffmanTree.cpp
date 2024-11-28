#include "HuffmanTree.h"

void HuffmanTree::createHuffmanTree() {
    while (q.size() > 1) {
        // 从二叉树森林中取权值最小的两颗二叉树,q指从小到大排列的优先节点队列
        HuffmanNode* left = new HuffmanNode(q.top());
        q.pop();
        HuffmanNode* right = new HuffmanNode(q.top());
        q.pop();

        // 将 left 和 right 作新节点的左右子树, 构造一个新的二叉树
        // 将新二叉树根节点的权值就是 left 和 right 两颗子树的权值之和,使用t来暂代节点
        HuffmanNode parent = HuffmanNode('t',left->freq + right->freq, left, right);
        // 将这个新的二叉树插入到二叉树森林中
        q.push(parent);
    }
}

unordered_map<char, string> HuffmanTree::createHuffmanCode() {
    string huffmanCode;
    unordered_map<char, string> charCode;
    createHuffmanTree();
    HuffmanNode root = q.top();
    subCreateHuffmanCode(&root,huffmanCode,charCode);
    return charCode;
}

void HuffmanTree::subCreateHuffmanCode(HuffmanNode* root, 
                string huffmancode, unordered_map<char,string>& charCode) {

    // if(isLeaf(root)) {
    //     charCode[root->data] = huffmancode;
    //     return;
    // }
    // //在分叉点复制一份相同的编码用于右子树
    // string right = huffmancode;
    // //是左子树加0
    // if(root->left != nullptr) {
    //     huffmancode += '0';
    //     subCreateHuffmanCode(root->left,huffmancode,charCode);
    // }
    // //是右子树加1
    // if(root->right!=nullptr){
    //     huffmancode = right;
    //     huffmancode += '1';
    //     subCreateHuffmanCode(root->right,huffmancode,charCode);
    // }

    //在分叉点复制一份相同的编码用于右子树
    string right = huffmancode;
    // 是左子树加0
    if (root->left != nullptr){
        huffmancode += '0';
        //如果此时其左孩子是叶子,就将其记录写入map
        if(isLeaf(root->left)){
            charCode[root->left->data] = huffmancode;
        }else{
            subCreateHuffmanCode(root->left,huffmancode,charCode);
        }
    }
    // 是右子树加1
    if(root->right!=nullptr){
        huffmancode = right;
        huffmancode += '1';
        //如果此时其右孩子是叶子,就将其记录写入map
        if(isLeaf(root->right)){
            charCode[root->right->data] = huffmancode;
        }
        else{
            subCreateHuffmanCode(root->right,huffmancode,charCode);
        }
    }
}

HuffmanNode* HuffmanTree::getHuffmanRoot() {
    return new HuffmanNode(q.top());
}
