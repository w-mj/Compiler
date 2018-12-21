//
// Created by wmj on 18-11-2.
//

#include "Utility.h"
#include "error/Error.h"
#include <cctype>
#include <stdexcept>
#include <iostream>
#include <sstream>

int hex2dec(char c) {
    if (isdigit(c)) return c - '0';
    if ('a' <= c && c <= 'f') return 10 + c - 'a';
    if ('A' <= c && c <= 'F') return 10 + c - 'A';
    throw "not a hexadecimal character.";
}

bool isalnumunder(int c) {
    return isalnum((c)) || (c) == '_';
}


using namespace std;

static void _split(const std::string &s, char delim,
                   std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    auto it = s.cbegin();
    auto end = s.cend();
    while (it != end) {
        string buf;
        while (it != end && *it != delim) {
            buf.push_back(*it);
            it++;
        }
        elems.push_back(buf);
        while (it != end && *it == delim) {
            it++;
        }
    }
    return elems;
}

#ifdef __linux__
void colorful_print(const std::string& s, int color, char end) {
    switch (color) {
        case 1:
            cout << "\033[1;31;0m" << s << "\033[0m" << end; break;
        case 2:
            cout << "\033[1;34;40m" << s << "\033[0m" << end; break;
        default:
            rterr("unsupported color" + to_string(color));
    }
}
#endif

#ifdef _WIN32
#include <windows.h>
#include <cstdio>
HANDLE hConsole = NULL;
void initConsole(void)
{
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == NULL) {
        fprintf(stderr, "初始化错误：无法获取控制台句柄\n");
        exit(1);
    }
    SetConsoleTitle("选课系统");
    COORD size = {120, 9001};  // 设置控制台缓冲
    SetConsoleScreenBufferSize(hConsole,size);
    SMALL_RECT rc =  {0, 0, 119, 29};  // 长宽的最大值为缓冲区减1
    SetConsoleWindowInfo(hConsole, true, &rc);
    char tt[1000];
    getcwd(tt, 1000);
    fputs(tt, stderr);
}
void colorful_print(const std::string& s, int color, char end) {
    if (hConsole == NULL) {
        initConsole();
    }
    SetConsoleTextAttribute(hConsole, color);
    cout << s << end;
    SetConsoleTextAttribute(hConsole, 0x0f);
}
#endif