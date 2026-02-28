#include "strlen.h"

using namespace KK::x86::String;

#include <iostream>
#include <windows.h>

int main() {
    const char* str = "01234567890123456789";
    std::size_t length = strlen(str);
    return 0;
}