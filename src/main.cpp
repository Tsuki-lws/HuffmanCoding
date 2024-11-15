// 程序运行处
#include "Utils.h"

int main() {
    int choice = getEncodeOrDecode();
    clock_t start = clock();
    Execution(choice);
    clock_t end = clock();
    cout << "压缩时间为:" << end - start << endl;
    return 0;
}