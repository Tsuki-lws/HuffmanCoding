#include "Utils.h"

void help(){
    cout << "=========================================================" << endl;
    cout << "=========  help command:   help                ==========" << endl;
    cout << "=========  exit command:   exit                ==========" << endl;
    cout << "=========  compress command:                   ==========" << endl;
    cout << "=========     hfm filename outputName          ==========" << endl;                
    cout << "=========     hfm filename outputName password ==========" << endl;       
    cout << "=========  decompress command:                 ==========" << endl;
    cout << "=========     unhfm filename                   ==========" << endl;                          
    cout << "=========     unhfm filename outputName        ==========" << endl;               
    cout << "=========================================================" << endl << endl;
}
void welcome(){
    cout << "=========================================================" << endl;
    cout << "===== Welcome to the compress & decompress program ======" << endl;
}
void start(){
    welcome();
    help();
    while(true){
        // 提示用户输入命令
        cout << "Enter command: " ;
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
        cout << endl;
        // 判断输入的命令是否合法
        if (command.size() > 0 && 
                (command[0] == "hfm" || command[0] == "unhfm" || command[0] == "exit" || command[0] == "help")) {
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
                try{
                    tool.decompress(filename,outputName,passLength);
                }catch(const runtime_error & e){
                    cout << "Decompression stopped" << endl;
                    continue;
                }
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
            }else if(command[0] == "help"){
                // 如果输入是 'exit'，则退出程序
                if (command.size() == 1) {
                    help();
                    continue;
                }else{
                    // 如果输入的命令不符合要求，输出错误提示
                    cerr << "Error: Unknown command '" 
                    << line << "'. Valid commands: 'hfm', 'unhfm', 'exit','help'." << endl;
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

