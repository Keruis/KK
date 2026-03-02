#pragma once

namespace KK::x86::Level {

enum class SimdLevel {
    Scalar,
    NativeWord,
    SSE,
    AVX2
};

}