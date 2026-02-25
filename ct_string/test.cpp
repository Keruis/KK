#include "ct_string.h"
#include <type_traits>

using namespace KK::Container::String;

int main() {
    using S0 = CTS("Hello"_cts);
    using S1 = CTS("World"_cts);

    static_assert(S0::size == 5);
    static_assert(!S0::empty);
    static_assert(!S0::equal<S1>);
    static_assert(S0::compare<CTS("Hellp"_cts)> == -1);
    static_assert(S0::at<0> == 'H');
    static_assert(std::is_same_v<S0::append<S1>, CTS("HelloWorld"_cts)>);
    static_assert(std::is_same_v<S0::prepend<S1>, CTS("WorldHello"_cts)>);
    static_assert(std::is_same_v<S0::substr<1, 2>, CTS("el"_cts)>);
    static_assert(S0::find<CTS("l"_cts)> == 2);
    static_assert(S0::contains<CTS("He"_cts)>);
    static_assert(std::is_same_v<S0::push_back<'w'>, CTS("Hellow"_cts)>);
    static_assert(std::is_same_v<S0::push_front<'w'>, CTS("wHello"_cts)>);
    static_assert(std::is_same_v<S0::pop_back<>, CTS("Hell"_cts)>);
    static_assert(std::is_same_v<S0::pop_front<>, CTS("ello"_cts)>);
    static_assert(std::is_same_v<S0::erase<0, 5>, CTS(""_cts)>);
    static_assert(std::is_same_v<S0::set<2, CTS("rr"_cts)>, CTS("Herro"_cts)>);
    static_assert(std::is_same_v<S0::insert<1, CTS("OP"_cts)>, CTS("HOPello"_cts)>);
    static_assert(std::is_same_v<S0::replace<CTS("lo"_cts), CTS("tyu"_cts)>, CTS("Heltyu"_cts)>);
    static_assert(S0::start_with<CTS("H"_cts)>);
    static_assert(S0::end_with<CTS("o"_cts)>);
}