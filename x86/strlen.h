#pragma once

#include <cstdint>

#include "util.h"

namespace KK::x86::String {

template <typename T>
[[nodiscard]] inline std::size_t strlen [[using gnu:always_inline, pure]](const T* ptr) noexcept;

template <typename T>
    requires (Util::is_any_of_v<T, char, char8_t>)
[[nodiscard]] inline std::size_t strlen [[using gnu:always_inline, pure]](const T* ptr) noexcept {
    std::size_t result;
    std::size_t base{};
    std::size_t block = reinterpret_cast<std::uintptr_t>(ptr) & -32;
    std::size_t addr  = reinterpret_cast<std::uintptr_t>(ptr);
    __asm__ inline (
        "   vpxor %%ymm0, %%ymm0, %%ymm0           ;    "
        "   vpcmpeqb (%[block]), %%ymm0, %%ymm1    ;    "
        "   vpmovmskb %%ymm1, %[out]               ;    "
        "   shrx %[shift], %[out], %[out]          ;    "
        "   test %[out], %[out]                    ;    "
        "   jnz 1f                                 ;    "

        "   add $64, %[block]                      ;    "

        "   2:                                     ;    "
        "   vpcmpeqb -32(%[block]), %%ymm0, %%ymm1 ;    "
        "   vpmovmskb %%ymm1, %[out]               ;    "
        "   test %[out], %[out]                    ;    "
        "   jnz 3f                                 ;    "
        "   vpcmpeqb (%[block]), %%ymm0, %%ymm1    ;    "
        "   vpmovmskb %%ymm1, %[out]               ;    "
        "   add $64, %[block]                      ;    "
        "   test %[out], %[out]                    ;    "
        "   jz 2b                                  ;    "
        "   sub $32, %[block]                      ;    "

        "   3:                                     ;    "
        "   sub %[addr], %[block]                  ;    "
        "   lea -32(%[block]), %[base]             ;    "

        "   1:                                     ;    "
        "   tzcnt %[out], %[out]                   ;    "
        "   add %[base], %[out]                    ;    "
        "   vzeroupper                             ;    "
        : [out] "=&r" (result), [block] "+r" (block), [base] "+r" (base)
        : [addr] "r" (addr), [shift]  "r"  (reinterpret_cast<std::uintptr_t>(ptr) & 31)
        : "ymm0", "ymm1", "cc"
    );

    return result;
}

}