# CS 3502 Section W03 Project One

## Overview
this project aims to demonstrate fundamental concepts in operating systems (OS)<br/>
the concepts that are coverd by each file are:
- `MultiThreading.cpp`: a program with a complete **multithreading** implementation
- `IPC.cpp`: a program with a **pipe** implementation

## Environment Setup for C++
the project requires an environment for a specific high-level programming language, `C++`<br/>

the recommended **IDE** for this project is [**Visual Studio Code**](https://code.visualstudio.com) 

in addition, it requires to isntall a C++ compiler externally

### For Linux 
install GNU C++ Compiler
```
sudo apt install build-essential 
```

check if the compiler is successfully installed
```
g++ -v 
```

then compile the program through a `terminal`
```
g++ -pthread -std=c++11 [file_name].cpp -o [any_name]
```
```
./[any_name]
```


### For MacOS
check if the compiler is already installed along with Xcode
```
clang --version
```
if the compiler is not installed, then install the compile 
```
xcode-select --install
```
then compile the program through a `terminal`
```
clang++ -pthread -std=c++11 [file_name].cpp -o [any_name]
```
```
./[any_name]
```

### For Windows
follow this [**instruction**](https://code.visualstudio.com/docs/cpp/config-mingw) from Visual Studio Code<br/>
the instruction guides to install the **MinGW-w64** toolchain to enable **GNU Compiler Collection**<br/><br/>
then, once installation is done, compile the program through a `terminal`
```
g++ -pthread -std=c++11 [file_name].cpp -o [any_name]
```
```
./[any_name]
```

## Required Inputs for the Project
the files -- `MultiThreading.cpp` and `IPC.cpp` -- require specific inputs to be successfully executed
### For MultiThreading.cpp
it requires two `int` inputs for
- number of Accounts
- number of threads

### For MultiThreading.cpp
it requires two 






