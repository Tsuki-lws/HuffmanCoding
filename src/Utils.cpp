#include "Utils.h"

// int getEncodeOrDecode()
// {
//     cout << "======================欢迎使用哈夫曼压缩&解压缩工具========================" << endl;
//     cout << "请选择你要使用的功能\n1.压缩\n2.解压缩" << endl;
//     while (1)
//     {
//         string line;
//         getline(cin, line);
//         if (line.size() == 1)
//         {
//             if (line == "1")
//             {
//                 return COMPRESS;
//             }
//             else if (line == "2")
//             {
//                 return DECOMPRESS;
//             }
//             else
//             {
//                 cout << "无效输入" << endl;
//             }
//         }
//         else
//         {
//             cout << "无效输入" << endl;
//         }
//     }
// }

void start(){
    while(true){
        // 提示用户输入命令
        cout << "Enter command (or 'exit' to quit): ";
        string line;
        getline(cin, line);

        // 如果用户什么都没输入，跳过本次循环
        if (line.empty()) {
            continue;
        }
        istringstream ss(line);
        vector<string> command;
        string word;
        while (ss >> word) {
            command.push_back(word);
        }

        // 判断输入的命令是否合法
        if (command.size() > 0 && 
                            (command[0] == "hfm" || command[0] == "unhfm" || command[0] == "exit")) {
            Features tool;
            // 根据命令读取相应的参数
            if (command[0] == "hfm") {
                string filename, outputName, password;
                if (command.size() < 3) {
                    cerr << "Error: 'hfm' requires at least two arguments: filename and outputName." << endl;
                    continue;
                }
                if(command.size() > 4) {
                    cerr << "Error: 'hfm' requires at last three arguments: filename, outputName and password." << endl;
                    continue;
                }
                // 获取文件名和输出名
                filename = command[1];
                fs::path path(filename);
                if (!fs::exists(path)) {
                    cout << "Filename invalid, please re-enter." << endl;
                    continue;
                }
                outputName = command[2];
                outputName += ".hfm"; 
                bool cover = true; // 默认为覆盖
                cover = checkCompressOutputPath(outputName);
                if(!cover){
                    cout << "please re-enter" <<endl;
                    continue;
                }
                // 获取密码（可选）
                if (command.size() > 3) {
                    password = command[3];
                }
                cout << "Compressing, please wait..." << endl;
                clock_t start = clock();
                tool.compress(filename,outputName,password);
                clock_t end = clock();
                cout << "Compression successful" << endl 
                << "Compression time: " << fixed << setprecision(2) 
                << (double)(end - start) / CLOCKS_PER_SEC << " seconds" << endl;
            } else if (command[0] == "unhfm") {
                string filename, outputName;
                if (command.size() < 2) {
                    cerr << "Error: 'unhfm' requires at least one argument: filename." << endl;
                    continue;
                }
                if (command.size() > 3) {
                    cerr << "Error: 'unhfm' requires at last two argument: filename and outputName." << endl;
                    continue;
                }

                // 获取文件名和输出名
                filename = command[1];
                fs::path path(filename);
                if (!fs::exists(path)) {
                    cerr << "Error: Invalid path, please re-enter." << endl;
                    continue;
                }else{
                    if(path.extension().string() != ".hfm"){
                        cerr << "Error: The file is not Huffman compressed, please re-enter." << endl;
                        continue;
                    }
                }
                outputName = (command.size() > 2) ? command[2] : "";
                int passLength = passwordCorrect(filename);
                cout << "Decompressing, please wait..." << endl;
                clock_t start = clock();
                tool.decompress(filename,outputName,passLength);
                clock_t end = clock();
                cout << "Decompression successful" << endl 
                << "Decompression time: " << fixed << setprecision(2) 
                << (double)(end - start) / CLOCKS_PER_SEC << " seconds" << endl;
            }
            else if(command[0] == "exit"){
                // 如果输入是 'exit'，则退出程序
                if (command.size() == 1) {
                    cout << "Exiting program." << endl;
                    break;
                }else{
                    // 如果输入的命令不符合要求，输出错误提示
                    cerr << "Error: Unknown command '" 
                    << line << "'. Valid commands: 'hfm', 'unhfm', 'exit'." << endl;
                }
            }
        }else{
            // 如果输入的命令不符合要求，输出错误提示
            cerr << "Error: Unknown command '" 
            << line << "'. Valid commands: 'hfm', 'unhfm', 'exit'." << endl;
        }
        cout << endl;
    }
}
// void Execution(int choice) {
//     Features tool;
//     switch(choice) {
//         case COMPRESS:
//         {
//             auto names = printInteraction(COMPRESS);
//             // 添加压缩密码
//             string password = encrypt();
//             clock_t start = clock();
//             cout << "压缩中,请等待" << endl;
//             tool.compress(names[0],names[1],password);
//             clock_t end = clock();
//             cout << "压缩成功" << endl << "压缩时间为:" << fixed << setprecision(2) << (double)(end - start) / CLOCKS_PER_SEC << "秒" << endl;
//             break;
//         }
//         case DECOMPRESS:
//         {
//             auto names = printInteraction(DECOMPRESS);
//             int passLength = passwordCorrect(names[0]);
//             clock_t start = clock();
//             cout << "解压缩中,请等待" << endl;
//             tool.decompress(names[0],names[1],passLength);
//             clock_t end = clock();
//             cout << "解压缩成功" << endl <<"解压缩时间为:" << fixed << setprecision(2) << (double)(end - start) / CLOCKS_PER_SEC << endl;
//             break;
//         }
//     }
// }
// // 简化交互
// string* printInteraction(int choice) {
//     static string name[2];
//     switch(choice) {
//         case COMPRESS:{
//             cout << "请输入你要压缩的文件路径(包含后缀)" << endl;
//             while(1){
//                 getline(cin,name[0]);
//                 fs::path path(name[0]);
//                 if (!fs::exists(path)) {
//                         cout << "无效的路径,请重新输入" << endl;
//                         continue;
//                     }
//                 break;
//             }
//             cout << "请输入你的目标文件名" << endl;
//             while(1){
//                 getline(cin,name[1]);
//                 name[1] += ".hfm"; 
//                 bool cover = true; // 默认为覆盖
//                 cover = checkCompressOutputPath(name[1]);
//                 if(!cover){
//                     cout << "请重新输入目标文件名" <<endl;
//                     continue;
//                 }
//                 break;
//             }
//             break;
//         }
//         case DECOMPRESS:{
//             cout << "请输入你要解压缩的文件路径(包含后缀)" << endl;
//             while(1){
//                 getline(cin,name[0]);
//                 fs::path path(name[0]);
                
//                 if (!fs::exists(path)) {
//                     cout << "无效的路径,请重新输入" << endl;
//                     continue;
//                 }else{
//                     if(path.extension().string() == ".hfm"){
//                         break;
//                     }else{
//                         cout << "文件不是经哈夫曼压缩的,请重新输入" << endl;
//                         continue;
//                     }
//                 }
//             }
//             break;
//         }
        
//     }
//     return name;
// }
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
// // 是否加密
// string encrypt(){
//     cout << "您是否要选择加密\n1.yes\n2.no" << endl;
//     string password = "";
//     while(1){
//         string s;
//         getline(cin,s);
//         if(s == "1"){
//             cout << "请输入密码" << endl;
//             getline(cin,password);
//             return password;
//         }else if(s == "2"){
//             return password;
//         }else{
//             cout << "格式不正确,请重新输入" << endl;
//             continue;
//         }
//     }
// }
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
        cout << "Please enter the password" << endl;
        while(1){
            string inputPassword = decode();
            if (inputPassword == correctPassword) {
                // return PasswordStatus::CORRECT;
                return passLength;
            } else {
                cerr << "Error: Incorrect password, please re-enter" << endl;
                continue;
            }
        }
    }
}

bool checkOutputPath(const string &filepath){
    if(fs::exists(filepath)){
        cout << filepath << " already exists, do you want to overwrite it?\n1. Overwrite\n2. Skip" << endl; 
        while(1){
            string line;
            getline(cin,line);
            if(line == "1"){ // 覆盖
                return true;
            }else if(line == "2"){ // 跳过
                cout << "Skipped" << endl;
                return false;
            }else{
                cerr << "Error: Invalid input, please re-enter" << endl;
                continue;
            }
        }
    }
    return true;
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