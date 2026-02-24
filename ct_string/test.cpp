#include "ct_string.h"

using namespace KK::Container::String;

int main() {
    using S0 = CTS("Hello"_cts);
    using S1 = CTS("World"_cts);

    static_assert(S0::size == 5);
    static_assert(!S0::empty);
    static_assert(!S0::equal<S1>);
    static_assert(S0::compare<CTS("Hellp"_cts)> == -1);
    static_assert(S0::at<0> == 'H');
    static_assert(S0::append<S1>::equal<CTS("HelloWorld"_cts)>);
    static_assert(S0::prepend<S1>::equal<CTS("WorldHello"_cts)>);
    static_assert(S0::substr<1, 2>::equal<CTS("el"_cts)>);
    static_assert(S0::find<CTS("l"_cts)> == 2);
    static_assert(S0::contains<CTS("He"_cts)>);
    static_assert(S0::push_back<'w'>::equal<CTS("Hellow"_cts)>);
    static_assert(S0::push_front<'w'>::equal<CTS("wHello"_cts)>);
    static_assert(S0::pop_back<>::equal<CTS("Hell"_cts)>);
    static_assert(S0::pop_front<>::equal<CTS("ello"_cts)>);
    static_assert(S0::erase<0, 5>::equal<CTS(""_cts)>);
    static_assert(S0::set<2, CTS("rr"_cts)>::equal<CTS("Herro"_cts)>);
    static_assert(S0::insert<1, CTS("OP"_cts)>::equal<CTS("HOPello"_cts)>);
    static_assert(S0::replace<CTS("lo"_cts), CTS("tyu"_cts)>::equal<CTS("Heltyu"_cts)>);
    static_assert(S0::start_with<CTS("H"_cts)>);
    static_assert(S0::end_with<CTS("o"_cts)>);
}