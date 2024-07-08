# Hermes
A super quick easy logger for C++
> [!INFO]
> Im working on this project alongside my other project [GFX](https://github.com/gloggers99/gfx), features are being added by necessity for that project. This is also why commits may have the same messages from GFX.
## Features
- [X] Cross Platform (Works on Windows/Linux/Mac)
- [X] Multiple Outputs At Once
- [X] Toggleable Multiplatform Colors
## Quick Start
Download the single header file in this project and place it in your project, then `#include <path/to/hermes.hpp>` inside your code!
### Usage
```c++
int main() {
    Hermes::Log("my log")("Hello World");

    return 0;
}
```
Or if you want a log object to pass around:
```c++
Hermes::Log myLog = Hermes::Log("my log");

int main() {
    myLog("Hello World");
    
    return 0;
}
```
### Different Ways To Print
Setting a stream to output to (`std::cerr` for example):
```c++
int main() {
    Hermes::Log myLog = Hermes::Log("my log", std::cerr);
    myLog("This is printing to the std::cerr stream")

    return 0;
}
```
Explicitly calling `log()`
```c++
Hermes::Log myLog = Hermes::Log("my log");

int main() {
    myLog.log("Hello World!");

    return 0;
}
```

