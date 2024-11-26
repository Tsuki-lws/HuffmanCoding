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
            // 添加压缩密码
            string password = encrypt();
            
            clock_t start = clock();
            cout << "压缩中,请等待" << endl;
            tool.compress(names[0],names[1],password);
            clock_t end = clock();
            cout << "压缩成功" << endl <<"压缩时间为:" << (end - start) / CLOCKS_PER_SEC << endl;
            break;
        }
        case DECOMPRESSFILE:
        case DECOMPRESSDIRECTORY:
        {
            auto names = printInteraction(DECOMPRESSFILE);
            int passLength = passwordCorrect(names[0]);
            clock_t start = clock();
            cout << "解压缩中,请等待" << endl;
            tool.decompress(names[0],names[1],passLength);
            clock_t end = clock();
            cout << "解压缩成功" << endl <<"解压缩时间为:" << (end - start) / CLOCKS_PER_SEC << endl;
            break;
        }
    }
}
// 简化交互
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
            cout << "请输入你要解压缩的文件路径(包含后缀)" << endl;
            getline(cin,name[0]);
            // cout << "请输入你要输出的文件夹名" << endl;
            // getline(cin,name[1]);
            break;
        }
    }
    return name;
}
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
    // string a;
    // getline(inputFile, a);
    for(int i = 0; i < filenameSize ; i++) {
        inputFile >> filesize[i];
        inputFile.get();
    }
    inputFile.close();

    return filesize;
}
// 是否加密
string encrypt(){
    cout << "您是否要选择加密\n1.y\n2.n" << endl;
    string password = "";
    while(1){
        string s;
        getline(cin,s);
        if(s.length() == 1){
            if(s == "y"){
                cout << "请输入密码" << endl;
                getline(cin,password);
                return password;
            }else if(s == "n"){
                return password;
            }else{
                cout << "格式不正确,请重新输入" << endl;
                continue;
            }
        }else{
            cout << "格式不正确,请重新输入" << endl;
            continue;
        }
    }
}
// 是否解密
string decode(){
    string password;
    getline(cin,password);
    return password;
}
// 判断密码是否正确,返回密码长度
int passwordCorrect(const string& filename) {
    ifstream input(filename,ios::in | ios::binary);
    int passLength;
    int result;
    input.read(reinterpret_cast<char *>(&passLength), sizeof(passLength));
    if(passLength == 0){
        // return PasswordStatus::NONE;
        return passLength;
    }else{
        string correctPassword(passLength, '\0');
        input.read(&correctPassword[0], passLength);
        cout << "请输入解压密码" << endl;
        while(1){
            string inputPassword = decode();
            if (inputPassword == correctPassword) {
                // return PasswordStatus::CORRECT;
                return passLength;
            } else {
                cout << "密码错误,请重新输入" << endl;
                continue;
            }
        }
    }
}