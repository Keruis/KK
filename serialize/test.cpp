#include <print>
#include "reflection.h"

enum Color {
    Red = 0,
    Blue = 1
};

struct B {
    int u;
    Color e;
};

struct A : B {
    char c;
    int i;
    double f;
    struct {
        int i;
    } s;
    std::array<int, 5> a;
    std::vector<int> v;
};

using namespace KK::Utils::Serialize;

int main() {
    A a{
        .c='x',
        .i=42,
        .f=9.81,
        .s {
            .i = 8
        },
        .a = {1, 2, 3, 4, 5},
        .v = {5, 4, 3, 2, 1}
    };
    a.u = 5;
    a.e = Color::Blue;

    {
        std::fstream file("test.bin", std::ios::out | std::ios::binary | std::ios::trunc);
        serialize(a, file);
    }

    A b{};
    {
        std::fstream file("test.bin", std::ios::in | std::ios::binary);
        deserialize(b, file);
    }

    std::println("b.c = {}", b.c);
    std::println("b.i = {}", b.i);
    std::println("b.f = {}", b.f);
    std::println("b.u = {}", b.u);
    std::println("b.e = {}", static_cast<int>(b.e));
    std::println("b.s.i = {}", b.s.i);
    for (auto n : b.a) std::print("{} ", n);
    std::println("");
    for (auto n : b.v) std::print("{} ", n);
    std::println("");
}