#include <iostream>

#include "strlen.h"

using namespace KK::x86::String;

int main() {
    const char* str = "0123456789";
    std::size_t length = strlen<KK::x86::Level::SimdLevel::AVX2>(str);
    std::cout << length << std::endl;    
    return 0;
}