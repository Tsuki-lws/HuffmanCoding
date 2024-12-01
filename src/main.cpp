// 程序运行处
#include "Utils.h"

int main(int argc, char* argv[]) {
    int choice = getEncodeOrDecode();
    Execution(choice);
    return 0;
}