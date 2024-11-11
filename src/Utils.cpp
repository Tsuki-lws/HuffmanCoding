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
        // case COMPRESSDIRECTORY:{
        //     string filename,outputName;
        //     auto names = printInteraction(COMPRESSDIRECTORY);
        //     filename = names[0];
        //     outputName = names[1];
        //     break;
        // }
        case DECOMPRESSFILE:
        case DECOMPRESSDIRECTORY:
        {
            auto names = printInteraction(DECOMPRESSFILE);
            tool.decompress(names[0],names[1]);
            break;
        }
        // case DECOMPRESSDIRECTORY:{
        //     string filename,outputName;
        //     auto names = printInteraction(DECOMPRESSDIRECTORY);
        //     filename = names[0];
        //     outputName = names[1];
        //     break;
        // }
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
            // Features tool;
            // tool.compress(name[0],name[1]);
            break;
        }
        case COMPRESSDIRECTORY:{
            cout << "请输入你要压缩的文件夹路径" << endl;
            getline(cin,name[0]);
            cout << "请输入你要输出的文件夹名(不用加后缀)" << endl;
            getline(cin,name[1]);
            break;
        }
        case DECOMPRESSFILE:{
            cout << "请输入你要解压缩的文件路径(包含后缀)" << endl;
            getline(cin,name[0]);
            cout << "请输入你要输出的文件文件名" << endl;
            getline(cin,name[1]);
            break;
        }
        case DECOMPRESSDIRECTORY:{
            cout << "请输入你要解压缩的文件路径" << endl;
            getline(cin,name[0]);
            cout << "请输入你要输出的文件夹名" << endl;
            getline(cin,name[1]);
            break;
        }
    }
    return name;
}