#include <iostream>
#include<queue>
#include <vector>
using namespace std;
class Node {
public:
    int freq;

    bool operator()(Node* const &n1, Node* const &n2){
        return  n1->freq > n2->freq; // 频率越高，优先级越低
    }
};

int main() {
    Node node1, node2;
    priority_queue<Node*,vector<Node*>,Node> q;
    node1.freq = 10;
    node2.freq = 20;
    q.push(&node1);
    q.push(&node2);

    std::cout << q.top()->freq << std::endl;

    return 0;
}