#ifndef CLI_H
#define CLI_H

#include<iostream>
#include<string>
#include <sstream>
#include <fstream>
#include "compress.h"
#include "decompress.h"
#include "HuffmanTree.h"
#include "Utils.h"

class CLI{
public:
    // 是否解密
    string decode();
    // 判断密码是否正确
    int passwordCorrect(const string& filename);
    // 启动程序,与用户交互
    void start();
    // 帮助菜单
    void help();
    // 欢迎界面
    void welcome();
    // 解析命令
    vector<string> parseCommand(const string& line);
    // 判断命令是否合法
    bool isValidCommand(const string& cmd);

    // 处理hfm命令
    void handleHfmCommand(const vector<string>& command, Compress& tool);
    // 处理unhfm命令
    void handleUnhfmCommand(const vector<string>& command, Decompress& tool);

    // 处理exit命令
    int handleExitCommand(const vector<string>& command,string line);
    // 处理help命令
    void handleHelpCommand(const vector<string>& command,string line);
};
#endif // CLI_H