# 基于哈夫曼编码的压缩和解压缩功能

## 项目结构

- `src/`
  - `Utils.h`: `Utils.cpp` 的头文件,定义了一个枚举类型。
  - `HuffmanTree.h`: `HuffmanTree.cpp` 的头文件,含有HuffmanTree类，HuffmanNode类。
  - `fileIO.h`: `fileIO.cpp` 的头文件,含有FileIO类,filehead和alphacode的struct，定义了蝗虫去大小BUFFER_SIZE。
  - `features.h`: `features.cpp` 的头文件,含有Features类。
  - `Utils.cpp`: 包含工具函数，如获取用户选择、执行功能等。
  - `main.cpp`: 程序入口，调用主要功能。
  - `HuffmanTree.cpp`: 实现哈夫曼树的创建和编码生成。
  - `fileIO.cpp`: 处理文件的读写、压缩和解压缩。
  - `features.cpp`: 定义压缩和解压缩的具体功能。

## 功能介绍

- 压缩文件和文件夹
- 解压缩文件和文件夹
- 加密(暂未实现)

## 使用方法

1. 运行 `main.cpp`。
2. 根据提示选择功能：
   - 1: 压缩文件
   - 2: 压缩文件夹
   - 3: 解压缩文件
   - 4: 解压缩文件夹
3. 输入相应的文件或文件夹路径。

## 依赖

- C++ 标准库
- `<filesystem>` 库


## 使用说明

- hfm 压缩指令
  - hfm filename outputName               将filename压缩到outputName
  - hfm filename outputName password      将filename压缩到outputName并设置压缩密码password
  - hfm DirectoryName outputName password 将DirectoryName压缩到outputName并设置压缩密码password

- unhfm 解压缩指令
  - unhfm filename                          解压缩filename到当前文件夹
  - unhfm filename outputName               将filename解压缩到outputName文件夹下
  <!-- - unhfm filename outputName password 以压缩密码password将filename解压缩到outputName -->





![image-20241207172252516](C:/Users/34829/AppData/Roaming/Typora/typora-user-images/image-20241207172252516.png)

![image-20241207172540970](C:/Users/34829/AppData/Roaming/Typora/typora-user-images/image-20241207172540970.png)

![image-20241207172822088](C:/Users/34829/AppData/Roaming/Typora/typora-user-images/image-20241207172822088.png)

如今
![image-20241207173423382](C:/Users/34829/AppData/Roaming/Typora/typora-user-images/image-20241207173423382.png)
