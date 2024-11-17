// 程序运行处
#include "Utils.h"

int main()
{
    int choice = getEncodeOrDecode();
    clock_t start = clock(); // 开始计时
    Execution(choice);
    clock_t end = clock();                                 // 结束计时
    int elapsed = int(end - start) / CLOCKS_PER_SEC; // 计算持续时间
    cout << "压缩时间为:" << elapsed << endl;
    return 0;
}