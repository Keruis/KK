#include "strlen.h"

using namespace KK::x86::String;

int main() {
    const char* str = "0123456789";
    std::size_t length = strlen(str);    
    return 0;
}