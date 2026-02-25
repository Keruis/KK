#pragma once

#include <meta>
#include <type_traits>
#include <fstream>

namespace KK::Utils::Reflection {

template <typename T> struct is_std_array : std::false_type {};
template <typename T, std::size_t N> struct is_std_array<std::array<T,N>> : std::true_type {};
template <typename T> constexpr bool is_std_array_v = is_std_array<T>::value;
template <typename T> struct is_std_vector : std::false_type {};
template <typename T> struct is_std_vector<std::vector<T>> : std::true_type {};
template <typename T> constexpr bool is_std_vector_v = is_std_vector<T>::value;
template <typename T> constexpr bool is_std_type = is_std_array_v<T> || is_std_vector_v<T>;

template <typename T>
    requires(std::is_arithmetic_v<T> || std::is_enum_v<T>)
struct BuildSerialize {
    static void serialize(const T& obj, std::fstream& stream) {
        stream.write(reinterpret_cast<const char*>(&obj), sizeof(T));
    }

    static void deserialize(T& obj, std::fstream& stream) {
        stream.read(reinterpret_cast<char*>(&obj), sizeof(T));
    }
};

template <typename T>
void serialize(T) { };
template <typename T>
void deserialize(T) { };

template <typename T>
    requires(std::is_arithmetic_v<T> || std::is_enum_v<T>)
void serialize(const T& obj, std::fstream& stream) {
    BuildSerialize<T>::serialize(obj, stream);
}

template <typename T>
    requires(is_std_array_v<T>)
void serialize(const T& obj, std::fstream& stream) {
    for (const auto& e : obj) {
        serialize(e, stream);
    }
}

template <typename T>
    requires(is_std_vector_v<T>)
void serialize(const T& obj, std::fstream& stream) {
    std::size_t s = obj.size();
    serialize(s, stream);
    for (const auto& e : obj) {
        serialize(e, stream);
    }
}

template <typename T>
    requires(std::is_class_v<T> && !is_std_type<T>)
void serialize(const T& obj, std::fstream& stream) {
    static constexpr auto reflection_T = ^^T;
    static constexpr auto ctx = std::meta::access_context::unchecked();

    static constexpr auto base_info_s = std::define_static_array(
        std::meta::bases_of(reflection_T, ctx)
    );

    static constexpr auto members_info_s = std::define_static_array(
        std::meta::nonstatic_data_members_of(reflection_T, ctx)
    );

    template for (constexpr auto info : base_info_s) {
        serialize(obj.[:info:], stream);
    }

    template for (constexpr auto info : members_info_s) {
        serialize(obj.[:info:], stream);
    }
}

template <typename T>
    requires(std::is_arithmetic_v<T> || std::is_enum_v<T>)
void deserialize(T& obj, std::fstream& stream) {
    BuildSerialize<T>::deserialize(obj, stream);
}

template <typename T>
    requires(is_std_array_v<T>)
void deserialize(T& obj, std::fstream& stream) {
    for (auto& e : obj) {
        deserialize(e, stream);
    }
}

template <typename T>
    requires(is_std_vector_v<T>)
void deserialize(T& obj, std::fstream& stream) {
    std::size_t s{};
    deserialize(s, stream);
    obj.resize(s);
    for (auto& e : obj) {
        deserialize(e, stream);
    }
}

template <typename T>
    requires(std::is_class_v<T> && !is_std_type<T>)
void deserialize(T& obj, std::fstream& stream) {
    static constexpr auto reflection_T = ^^T;
    static constexpr auto ctx = std::meta::access_context::unchecked();

    static constexpr auto base_info_s = std::define_static_array(
        std::meta::bases_of(reflection_T, ctx)
    );

    static constexpr auto members_info_s = std::define_static_array(
        std::meta::nonstatic_data_members_of(reflection_T, ctx)
    );

    template for (constexpr auto info : base_info_s) {
        deserialize(obj.[:info:], stream);
    }

    template for (constexpr auto info : members_info_s) {
        deserialize(obj.[:info:], stream);
    }
}

}