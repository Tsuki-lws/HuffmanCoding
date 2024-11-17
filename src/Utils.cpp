#include "Utils.h"

int getEncodeOrDecode()
{
    cout << "======================欢迎使用哈夫曼压缩&解压缩工具========================" << endl;
    cout << "请选择你要使用的功能\n1.压缩文件\n2.压缩文件夹\n3.解压缩文件\n4.解压缩文件夹" << endl;
    while (1)
    {
        string line;
        getline(cin, line);
        if (line.size() == 1)
        {
            if (line == "1")
            {
                return COMPRESSFILE;
            }
            else if (line == "2")
            {
                return COMPRESSDIRECTORY;
            }
            else if (line == "3")
            {
                return DECOMPRESSFILE;
            }
            else if (line == "4")
            {
                return DECOMPRESSDIRECTORY;
            }
            else
            {
                cout << "无效输入" << endl;
            }
        }
    }
}

void Execution(int choice) {
    Features tool;
    switch(choice) {
        case COMPRESSFILE:
        case COMPRESSDIRECTORY:
        {
            auto names = printInteraction(COMPRESSFILE);
            tool.compress(names[0],names[1]);
            break;
        }
        case DECOMPRESSFILE:
        {
            auto names = printInteraction(DECOMPRESSFILE);
            tool.decompress(names[0],names[1]);
            break;
        }
        case DECOMPRESSDIRECTORY:
        {
            auto names = printInteraction(DECOMPRESSFILE);
            tool.decompressDir(names[0]);
            break;
        }
    }
}

string* printInteraction(int choice) {
    static string name[2];
    switch(choice) {
        case COMPRESSFILE:{
            cout << "请输入你要压缩的文件路径(包含后缀)" << endl;
            getline(cin,name[0]);
            cout << "请输入你要输出的文件文件名(不用加后缀)" << endl;
            getline(cin,name[1]);
            name[1] += ".huffman"; 
            break;
        }
        case COMPRESSDIRECTORY:{
            cout << "请输入你要压缩的文件夹路径" << endl;
            getline(cin,name[0]);
            cout << "请输入你要输出的文件夹名(不用加后缀)" << endl;
            getline(cin,name[1]);
            name[1] += ".huffman"; 
            break;
        }
        case DECOMPRESSFILE:{
            cout << "请输入你要解压缩的文件路径(包含后缀)" << endl;
            getline(cin,name[0]);
            // cout << "请输入你要输出的文件文件名" << endl;
            // getline(cin,name[1]);
            break;
        }
        case DECOMPRESSDIRECTORY:{
            cout << "请输入你要解压缩的文件路径" << endl;
            getline(cin,name[0]);
            // cout << "请输入你要输出的文件夹名" << endl;
            // getline(cin,name[1]);
            break;
        }
    }
    return name;
}

long long* getCompressDirSize(const string& filename, int filenameSize){
    ifstream inputFile(filename, ios::in | ios::ate);
    long long* filesize = new long long[filenameSize];
    string line;

    // 从文件末尾开始向前查找 '\n'
    // 查看前一个字符是否为回车符
    while (inputFile.peek() != '\n'){
        inputFile.seekg(-1, inputFile.cur);
    }
    inputFile.seekg(2, inputFile.cur);

    // 直接读取数字到filesize数组中
    
    for(int i = 0; i < filenameSize ; i++) {
        inputFile >> filesize[i];
    }
    inputFile.close();

    return filesize;
}