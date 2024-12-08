#include "CLI.h"
#include "Utils.h"


void CLI::help(){
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
void CLI::welcome(){
    cout << "=========================================================" << endl;
    cout << "===== Welcome to the compress & decompress program ======" << endl;
}

void CLI::start() {
    welcome();
    help();
    while (true) {
        cout << "Enter command: ";
        string line;
        getline(cin, line);
        if (any_of(line.begin(), line.end(), [](unsigned char c){ return c >= 0x80; })) {
            cerr << "Error: Command contains non-ASCII characters. Please enter a valid command." << endl;
            continue;
        }
        if (line.empty()) {
            continue;
        }
        vector<string> command = parseCommand(line);
        if (command.empty()) {
            continue;
        }
        if (isValidCommand(command[0])) {
            if (command[0] == "hfm") {
                Compress tool;
                handleHfmCommand(command, tool);
            } else if (command[0] == "unhfm") {
                Decompress tool;
                handleUnhfmCommand(command, tool);
            } else if (command[0] == "exit") {
                int result = handleExitCommand(command,line);
                if(result){
                    break;
                }
            } else if (command[0] == "help") {
                handleHelpCommand(command,line);
            }
        } else {
            cerr << "Error: Unknown command '" << line << "'. Valid commands: 'hfm', 'unhfm', 'exit', 'help'." << endl;
        }
        cout << endl;
    }
}

vector<string> CLI::parseCommand(const string& line) {
    istringstream ss(line);
    vector<string> command;
    string word;
    while (ss >> word) {
        command.push_back(word);
    }
    return command;
}
// 判断命令是否合法
bool CLI::isValidCommand(const string& cmd) {
    return cmd == "hfm" || cmd == "unhfm" || cmd == "exit" || cmd == "help";
}
// 处理hfm命令
void CLI::handleHfmCommand(const vector<string>& command, Compress& tool) {
    if (command.size() < 3) {
        cerr << "Error: 'hfm' requires at least two arguments: filename and outputName." << endl;
        return;
    }
    if (command.size() > 4) {
        cerr << "Error: 'hfm' requires at last three arguments: filename, outputName and password." << endl;
        return;
    }
    string filename = command[1];
    fs::path path = fs::u8path(filename);
    if (!fs::exists(path)) {
        cout << "Filename invalid, please re-enter." << endl;
        return;
    }
    string outputName = command[2] + ".hfm";
    if (!checkCompressOutputPath(outputName)) {
        cout << "please re-enter" << endl;
        return;
    }
    string password = (command.size() > 3) ? command[3] : "";
    cout << "Compressing, please wait..." << endl;
    clock_t start = clock();
    tool.compress(filename, outputName, password);
    clock_t end = clock();

    cout << "Compression successful" << endl
         << "Compression time: " << fixed << setprecision(2)
         << (double)(end - start) / CLOCKS_PER_SEC << " seconds" << endl;
}
// 处理unhfm命令
void CLI::handleUnhfmCommand(const vector<string>& command, Decompress& tool) {
    if (command.size() < 2) {
        cerr << "Error: 'unhfm' requires at least one argument: filename." << endl;
        return;
    }
    if (command.size() > 3) {
        cerr << "Error: 'unhfm' requires at last two arguments: filename and outputName." << endl;
        return;
    }

    string filename = command[1];
    fs::path path(filename);
    if (!fs::exists(path)) {
        cerr << "Error: Invalid filenamepath, please re-enter." << endl;
        return;
    }
    if (path.extension().string() != ".hfm") {
        cerr << "Error: The file is not Huffman compressed, please re-enter." << endl;
        return;
    }

    string outputName = (command.size() > 2) ? command[2] : "";
    int passLength = passwordCorrect(filename);
    cout << "Decompressing, please wait..." << endl;
    clock_t start = clock();
    try {
        tool.decompress(filename, outputName, passLength);
    } catch (const runtime_error& e) {
        cout << "Decompression stopped" << endl;
        return;
    }
    clock_t end = clock();
    cout << "Decompression successful" << endl
         << "Decompression time: " << fixed << setprecision(2)
         << (double)(end - start) / CLOCKS_PER_SEC << " seconds" << endl;
}
// 处理exit命令
int CLI::handleExitCommand(const vector<string>& command, string line) {
    if (command.size() == 1) {
        cout << "Thank you for using the program. Goodbye!" << endl;
        return 1;
    } else {
        cerr << "Error: Unknown command '" << line << "'. Valid commands: 'hfm', 'unhfm', 'exit'." << endl;
        return 0;
    }
}
// 处理help命令
void CLI::handleHelpCommand(const vector<string>& command,string line) {
    if (command.size() == 1) {
        help();
    } else {
        cerr << "Error: Unknown command '" << line 
            << "'. Valid commands: 'hfm', 'unhfm', 'exit', 'help'." << endl;
    }
}
// 是否解密
string CLI::decode(){
    string password;
    getline(cin,password);
    return password;
}
// 判断密码是否正确,返回密码长度
int CLI::passwordCorrect(const string& filename) {
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
