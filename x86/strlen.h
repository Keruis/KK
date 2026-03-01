#pragma once

#include <cstdint>
#include <immintrin.h>

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
    __m256i zero = _mm256_setzero_si256();
    __m256i tv;
    __asm__ inline (
        "   vpcmpeqb (%[block]), %[zero], %[tv]    ;    "
        "   vpmovmskb %[tv], %[out]                ;    "
        "   shrx %[shift], %[out], %[out]          ;    "
        "   test %[out], %[out]                    ;    "
        "   jnz 1f                                 ;    "

        "   add $64, %[block]                      ;    "

        "   2:                                     ;    "
        "   vpcmpeqb -32(%[block]), %[zero], %[tv] ;    "
        "   vpmovmskb %[tv], %[out]                ;    "
        "   test %[out], %[out]                    ;    "
        "   jnz 3f                                 ;    "
        "   vpcmpeqb (%[block]), %[zero], %[tv]    ;    "
        "   vpmovmskb %[tv], %[out]                ;    "
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
        : [out] "=&r" (result), [block] "+r" (block), [base] "+r" (base), [tv] "=&v" (tv)
        : [addr] "r" (addr), [shift]  "r"  (reinterpret_cast<std::uintptr_t>(ptr) & 31), [zero] "v" (zero)
        : "cc"
    );

    return result;
}

template <typename T>
    requires (Util::is_any_of_v<T, wchar_t, char16_t>)
[[nodiscard]] inline std::size_t strlen [[using gnu:always_inline, pure]](const T* ptr) noexcept {
    std::size_t result;
    std::size_t base{};
    std::size_t block = reinterpret_cast<std::uintptr_t>(ptr) & -32;
    std::size_t addr  = reinterpret_cast<std::uintptr_t>(ptr);
    __m256i zero = _mm256_setzero_si256();
    __m256i tv;
    __asm__ inline (
        "   vpcmpeqw (%[block]), %[zero], %[tv]    ;    "
        "   vpmovmskb %[tv], %[out]                ;    "
        "   shrx %[shift], %[out], %[out]          ;    "
        "   test %[out], %[out]                    ;    "
        "   jnz 1f                                 ;    "

        "   add $64, %[block]                      ;    "

        "   2:                                     ;    "
        "   vpcmpeqw -32(%[block]), %[zero], %[tv] ;    "
        "   vpmovmskb %[tv], %[out]                ;    "
        "   test %[out], %[out]                    ;    "
        "   jnz 3f                                 ;    "
        "   vpcmpeqw (%[block]), %[zero], %[tv]    ;    "
        "   vpmovmskb %[tv], %[out]                ;    "
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
        "   shr $1, %[out]                         ;    "
        : [out] "=&r" (result), [block] "+r" (block), [base] "+r" (base), [tv] "=&v" (tv)
        : [addr] "r" (addr), [shift]  "r"  (reinterpret_cast<std::uintptr_t>(ptr) & 31), [zero] "v" (zero)
        : "cc"
    );

    return result;
}

template <typename T>
    requires (Util::is_any_of_v<T, char32_t>)
[[nodiscard]] inline std::size_t strlen [[using gnu:always_inline, pure]](const T* ptr) noexcept {
    std::size_t result;
    std::size_t base{};
    std::size_t block = reinterpret_cast<std::uintptr_t>(ptr) & -32;
    std::size_t addr  = reinterpret_cast<std::uintptr_t>(ptr);
    __m256i zero = _mm256_setzero_si256();
    __m256i tv;
    __asm__ inline (
        "   vpcmpeqd (%[block]), %[zero], %[tv]    ;    "
        "   vpmovmskb %[tv], %[out]                ;    "
        "   shrx %[shift], %[out], %[out]          ;    "
        "   test %[out], %[out]                    ;    "
        "   jnz 1f                                 ;    "

        "   add $64, %[block]                      ;    "

        "   2:                                     ;    "
        "   vpcmpeqd -32(%[block]), %[zero], %[tv] ;    "
        "   vpmovmskb %[tv], %[out]                ;    "
        "   test %[out], %[out]                    ;    "
        "   jnz 3f                                 ;    "
        "   vpcmpeqd (%[block]), %[zero], %[tv]    ;    "
        "   vpmovmskb %[tv], %[out]                ;    "
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
        "   shr $2, %[out]                         ;    "
        : [out] "=&r" (result), [block] "+r" (block), [base] "+r" (base), [tv] "=&v" (tv)
        : [addr] "r" (addr), [shift]  "r"  (reinterpret_cast<std::uintptr_t>(ptr) & 31), [zero] "v" (zero)
        : "cc"
    );

    return result;
}


}