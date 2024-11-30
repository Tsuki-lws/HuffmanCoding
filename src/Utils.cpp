#include "Utils.h"

int getEncodeOrDecode()
{
    cout << "======================欢迎使用哈夫曼压缩&解压缩工具========================" << endl;
    cout << "请选择你要使用的功能\n1.压缩\n2.解压缩" << endl;
    while (1)
    {
        string line;
        getline(cin, line);
        if (line.size() == 1)
        {
            if (line == "1")
            {
                return COMPRESS;
            }
            else if (line == "2")
            {
                return DECOMPRESS;
            }
            else
            {
                cout << "无效输入" << endl;
            }
        }
        else
        {
            cout << "无效输入" << endl;
        }
    }
}

void Execution(int choice) {
    Features tool;
    switch(choice) {
        case COMPRESS:
        {
            auto names = printInteraction(COMPRESS);
            // 添加压缩密码
            string password = encrypt();
            
            clock_t start = clock();
            cout << "压缩中,请等待" << endl;
            tool.compress(names[0],names[1],password);
            clock_t end = clock();
            cout << "压缩成功" << endl << "压缩时间为:" << fixed << setprecision(2) << (double)(end - start) / CLOCKS_PER_SEC << "秒" << endl;
            break;
        }
        case DECOMPRESS:
        {
            auto names = printInteraction(DECOMPRESS);
            int passLength = passwordCorrect(names[0]);
            clock_t start = clock();
            cout << "解压缩中,请等待" << endl;
            tool.decompress(names[0],names[1],passLength);
            clock_t end = clock();
            cout << "解压缩成功" << endl <<"解压缩时间为:" << fixed << setprecision(2) << (double)(end - start) / CLOCKS_PER_SEC << endl;
            break;
        }
    }
}
// 简化交互
string* printInteraction(int choice) {
    static string name[2];
    switch(choice) {
        case COMPRESS:{
            cout << "请输入你要压缩的文件路径(包含后缀)" << endl;
            while(1){
                getline(cin,name[0]);
                fs::path path(name[0]);
                if (!fs::exists(path)) {
                        cout << "无效的路径,请重新输入" << endl;
                        continue;
                    }
                break;
            }
            cout << "请输入你的目标文件名" << endl;
            while(1){
                getline(cin,name[1]);
                name[1] += ".huf"; 
                bool cover = true; // 默认为覆盖
                cover = checkCompressOutputPath(name[1]);
                if(!cover){
                    cout << "请重新输入目标文件名" <<endl;
                    continue;
                }
                break;
            }
            break;
        }
        case DECOMPRESS:{
            cout << "请输入你要解压缩的文件路径(包含后缀)" << endl;
            while(1){
                getline(cin,name[0]);
                fs::path path(name[0]);
                
                if (!fs::exists(path)) {
                    cout << "无效的路径,请重新输入" << endl;
                    continue;
                }else{
                    if(path.extension().string() == ".huf"){
                        break;
                    }else{
                        cout << "文件不是经哈夫曼压缩的,请重新输入" << endl;
                        continue;
                    }
                }
            }
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
    cout << "您是否要选择加密\n1.yes\n2.no" << endl;
    string password = "";
    while(1){
        string s;
        getline(cin,s);
        if(s == "1"){
            cout << "请输入密码" << endl;
            getline(cin,password);
            return password;
        }else if(s == "2"){
            return password;
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

bool checkOutputPath(const string &filepath){
    if(fs::exists(filepath)){
        cout << filepath << "已存在,是否要覆盖\n1.覆盖\n2.跳过" << endl; 
        while(1){
            string line;
            getline(cin,line);
            if(line == "1"){ // 覆盖
                return true;
            }else if(line == "2"){ // 跳过
                cout << "已跳过" << endl;
                return false;
            }else{
                cout << "无效输入,请重新输入" << endl;
                continue;
            }
        }
    }
    return true;
}
bool checkCompressOutputPath(const string &filepath){
    if(fs::exists(filepath)){
        cout << filepath <<"已存在,是否要覆盖\n1.覆盖\n2.选择其他路径" << endl; 
        while(1){
            string line;
            getline(cin,line);
            if(line == "1"){ // 覆盖
                return true;
            }else if(line == "2"){ // 选择其他路径
                return false;
            }else{
                cout << "无效输入,请重新输入" << endl;
                continue;
            }
        }
    }
    return true;
}