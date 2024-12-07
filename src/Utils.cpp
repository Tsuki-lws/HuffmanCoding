#include "Utils.h"


// 获得每个文件的压缩文件的大小
long long* getCompressDirSize(const string& filename, int filenameSize){
    ifstream inputFile(filename, ios::in | ios::ate);
    long long* filesize = new long long[filenameSize];
    string line;

    // 从文件末尾开始向前查找 '\n'
    // 查看前一个字符是否为回车符
    while (inputFile.peek() != '\n'){
        inputFile.seekg(-1, inputFile.cur);
    }
    inputFile.seekg(1, inputFile.cur);

    // 直接读取数字到filesize数组中
    for(int i = 0; i < filenameSize ; i++) {
        inputFile >> filesize[i];
        inputFile.get();
    }
    inputFile.close();

    return filesize;
}

int checkOutputPath(const string &filepath){
    if(fs::exists(filepath)){
        cout << filepath << " already exists, do you want to overwrite it?\n"<<
                                    "1. Overwrite\n2. Skip\n3.exit" << endl; 
        while(1){
            string line;
            getline(cin,line);
            if(line == "1"){ // 覆盖
                return coverStatus::OVERWRITE;
            }else if(line == "2"){ // 跳过
                cout << "Skipped" << endl;
                return coverStatus::SKIP;
            }else if(line == "3"){
                return coverStatus::EXIT;
            }else{
                cerr << "Error: Invalid input, please re-enter" << endl;
                continue;
            }
        }
    }
    return true;
}
int coverAll(const vector<string> &filepath, int filenameSize){
    int count = 0;
    for(int i = 0; i < filenameSize; i++) {
        if(fs::exists(filepath[i])){
            count++;
        }
        if(count > 3){
            break;
        }
    }
    if(count > 3){
        cout << "More than 10 identical files found, do you want to overwrite all?\n"<<
                            "1. Overwrite all\n2. Decide individually\n3.Skip all" << endl;
        while(1){
            string line;
            getline(cin,line);
            if(line == "1"){ // 全部覆盖
                return coverStatus::OVERWRITE;
            }else if(line == "2"){ // 自己决定
                return coverStatus::OTHER;
            }else if(line == "3"){
                return coverStatus::SKIP;
            }else{
                cerr << "Error: Invalid input, please re-enter" << endl;
                continue;
            }
        }
    }
    // 默认自己决定
    return coverStatus::OTHER;
}

bool checkCompressOutputPath(const string &filepath){
    if(fs::exists(filepath)){
        cout << filepath << " already exists, do you want to overwrite it?\n1. Overwrite\n2. Choose another path" << endl; 
        while(1){
            string line;
            getline(cin,line);
            if(line == "1"){ // 覆盖
                return true;
            }else if(line == "2"){ // 选择其他路径
                return false;
            }else{
                cerr << "Error: Invalid input, please re-enter" << endl;
                continue;
            }
        }
    }
    return true;
}

