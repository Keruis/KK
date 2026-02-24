#pragma once

#define STD_VERSION_26 202600L
#define STD_VERSION_24 202400L
#define STD_VERSION_23 202302L
#define STD_VERSION_20 202002L
#define STD_VERSION_17 201703L

#if __cplusplus >= STD_VERSION_20
    #define COMPILE_TIME                  consteval
    #define SPECIALIZATION
    #define SPECIALIZATION_PARAMETER(P)
    #define SPECIALIZATION_IMPL(E)
    #define CONSTRAINT(E)                 > requires(E)
    #define REQUIRES(E)                   requires(E)
#else
    #define COMPILE_TIME                  constexpr
    #define SPECIALIZATION                , typename = void
    #define SPECIALIZATION_PARAMETER(P)   , P
    #define SPECIALIZATION_IMPL(E)        , Util::enable_if_t<(E)>
    #define CONSTRAINT(E)                 , typename = Util::enable_if_t<(E)>>
    #define REQUIRES(E)
#endif

namespace KK::Container::String {

namespace Util {
    using size_t = unsigned long long;

    template <bool B, typename T = void>
    struct enable_if {};
    template <typename T>
    struct enable_if<true, T> { using type = T; };
    template <bool B, typename T = void>
    using enable_if_t = typename enable_if<B, T>::type;

    template <typename T1, typename T2>
    struct is_same { static constexpr bool value = false; };
    template <typename T>
    struct is_same<T, T> { static constexpr bool value = true; };
    template <typename T1, typename T2>
    static constexpr bool is_same_v = is_same<T1, T2>::value;

    template <bool B, typename T, typename F>
    struct conditional { using type = T; };
    template <typename T, typename F>
    struct conditional<false, T, F> { using type = F; };
    template <bool B, typename T, typename F>
    using conditional_t = typename conditional<B, T, F>::type;

    template <typename T>
    struct type_identity { using type = T; };
}

template <char...>
struct ct_string;

template <typename CT, CT... Cs>
COMPILE_TIME auto operator""_cts() {
    return Util::type_identity<ct_string<Cs...>>{};
}

#define CTS(str) typename decltype(str)::type

template <typename>
struct ct_size;
template <typename, Util::size_t SPECIALIZATION>
struct ct_at;
template <typename, typename...>
struct ct_concat;
template <typename, Util::size_t, Util::size_t SPECIALIZATION, Util::size_t...>
struct ct_slice;
template <typename, typename...>
struct ct_equal;
template <typename, typename>
struct ct_compare;
template <typename, typename>
struct ct_find;
template <typename, Util::size_t, typename SPECIALIZATION>
struct ct_set;
template <typename, Util::size_t, typename>
struct ct_insert;
template <typename, Util::size_t, Util::size_t SPECIALIZATION>
struct ct_erase;
template <typename, typename, typename SPECIALIZATION>
struct ct_replace;
template <typename, typename>
struct ct_start_with;
template <typename, typename>
struct ct_end_with;

template <char... Cs>
struct ct_size<ct_string<Cs...>> {
    static constexpr Util::size_t size = sizeof...(Cs);
};

template <char... Cs, Util::size_t I>
struct ct_at<ct_string<Cs...>, I SPECIALIZATION_IMPL(I < sizeof...(Cs))> {
    static constexpr char value = []{
#if __cplusplus < STD_VERSION_24
        constexpr char arr[] = { Cs... };
        return arr[I];
#else
        return Cs...[I];
#endif
    }();
};

template <char... Cs1, char... Cs2>
struct ct_concat<ct_string<Cs1...>, ct_string<Cs2...>> {
    using type = ct_string<Cs1..., Cs2...>;
};

template <char... Cs1, char... Cs2, typename... Rest>
struct ct_concat<ct_string<Cs1...>, ct_string<Cs2...>, Rest...> {
    using type =
        typename ct_concat<
            typename ct_concat<ct_string<Cs1...>, ct_string<Cs2...>>::type,
            Rest...
        >::type;
};

template <Util::size_t P, Util::size_t C, char... Cs, Util::size_t... Is>
struct ct_slice<ct_string<Cs...>, P, C SPECIALIZATION_IMPL(P + C <= sizeof...(Cs)), Is...> {
    using slice = ct_string<ct_at<ct_string<Cs...>, P + Is>::value...>;
};

template <typename Self, typename... Others>
struct ct_equal {
    static constexpr bool equal = (Util::is_same_v<Self, Others> && ...);
};

template <char... Cs1, char... Cs2>
struct ct_compare<ct_string<Cs1...>, ct_string<Cs2...>> {
    static constexpr int value = [](){
        constexpr char arr_1[] = { Cs1... };
        constexpr char arr_2[] = { Cs2... };
        constexpr Util::size_t N_1 = sizeof...(Cs1);
        constexpr Util::size_t N_2 = sizeof...(Cs2);
        constexpr Util::size_t min_size = N_1 < N_2 ? N_1 : N_2;
        for (int i = 0; i < min_size; i++) {
            if (arr_1[i] < arr_2[i]) return -1;
            if (arr_1[i] > arr_2[i]) return 1;
        }
        if (N_1 < N_2) return -1;
        if (N_1 > N_2) return 1;
        return 0;
    }();
};

template <char... Cs, char... SCs>
struct ct_find<ct_string<Cs...>, ct_string<SCs...>> {
    static constexpr Util::size_t pos = [](){
        constexpr char arr_s[] = {Cs...};
        constexpr char arr_sub[] = {SCs...};
        constexpr Util::size_t N = sizeof...(Cs);
        constexpr Util::size_t M = sizeof...(SCs);
        for (Util::size_t i = 0; i + M <= N; i++) {
            bool matched = true;
            for (Util::size_t j = 0; j < M; j++) {
                if (arr_s[i + j] != arr_sub[j]) {
                    matched = false;
                    break;
                }
            }
            if (matched) return i;
        }
        return static_cast<Util::size_t>(-1);
    }();
};

template <char... Cs, Util::size_t I, char... SCs>
struct ct_set<ct_string<Cs...>, I, ct_string<SCs...> SPECIALIZATION_IMPL(sizeof...(SCs) != 0 && (I + sizeof...(SCs) <= sizeof...(Cs)))> {
private:
    static constexpr Util::size_t N = sizeof...(Cs);
    static constexpr Util::size_t M = sizeof...(SCs);

    using prefix = typename ct_string<Cs...>::substr<0, I>;
    using suffix = typename ct_string<Cs...>::substr<I + M, N - I - M>;

public:
    using type = typename ct_concat<prefix, ct_string<SCs...>, suffix>::type;
};

template <char... Cs, Util::size_t I, char... SCs>
struct ct_insert<ct_string<Cs...>, I, ct_string<SCs...>> {
private:
    static constexpr Util::size_t N = sizeof...(Cs);
    static constexpr Util::size_t M = sizeof...(SCs);

    using prefix = typename ct_string<Cs...>::substr<0, I>;
    using suffix = typename ct_string<Cs...>::substr<I, N - I>;

public:
    using type = typename ct_concat<prefix, ct_string<SCs...>, suffix>::type;
};

template <char... Cs, Util::size_t P, Util::size_t C>
struct ct_erase<ct_string<Cs...>, P, C SPECIALIZATION_IMPL(P + C <= sizeof...(Cs))> {
private:
    static constexpr Util::size_t N = sizeof...(Cs);

    using prefix = typename ct_string<Cs...>::substr<0, P>;
    using suffix = typename ct_string<Cs...>::substr<P + C, N - P - C>;

public:
    using type = typename ct_concat<prefix, suffix>::type;
};

template <char... Cs, char... FCs, char... TCs>
struct ct_replace<ct_string<Cs...>, ct_string<FCs...>, ct_string<TCs...>
    SPECIALIZATION_IMPL((ct_find<ct_string<Cs...>, ct_string<FCs...>>::pos != static_cast<Util::size_t>(-1)))> {
private:
    using self = ct_string<Cs...>;
    using from = ct_string<FCs...>;
    using to   = ct_string<TCs...>;

    static constexpr Util::size_t pos = ct_find<self, from>::pos;

public:
    using type = typename ct_insert<
        typename ct_erase<self, pos, ct_size<from>::size>::type,
        pos,
        to
    >::type;
};

template <char... Cs, char... FCs, char... TCs>
    REQUIRES((ct_find<ct_string<Cs...>, ct_string<FCs...>>::pos == static_cast<Util::size_t>(-1)))
struct ct_replace<ct_string<Cs...>, ct_string<FCs...>, ct_string<TCs...>
    SPECIALIZATION_IMPL((ct_find<ct_string<Cs...>, ct_string<FCs...>>::pos == static_cast<Util::size_t>(-1)))> {
private:
    using self = ct_string<Cs...>;

public:
    using type = self;
};

template <char... Cs, char... SCs>
struct ct_start_with<ct_string<Cs...>, ct_string<SCs...>> {
private:
    static constexpr Util::size_t N = sizeof...(Cs);
    static constexpr Util::size_t M = sizeof...(SCs);

    template <Util::size_t... Is>
    static constexpr bool impl() {
        return ((ct_at<ct_string<Cs...>, Is>::value == ct_at<ct_string<SCs...>, Is>::value) && ...);
    }

public:
    static constexpr bool value = (M <= N) && impl<__integer_pack(M)...>();
};

template <char... Cs, char... SCs>
struct ct_end_with<ct_string<Cs...>, ct_string<SCs...>> {
private:
    static constexpr Util::size_t N = sizeof...(Cs);
    static constexpr Util::size_t M = sizeof...(SCs);

    template <Util::size_t... Is>
    static constexpr bool impl() {
        return ((ct_at<ct_string<Cs...>, N - M + Is>::value == ct_at<ct_string<SCs...>, Is>::value) && ...);
    }

public:
    static constexpr bool value = (M <= N) && impl<__integer_pack(M)...>();
};

namespace Util {
    using NEW_LINE         = ct_string<'\n'>;
    using ANSI_RESET       = ct_string<'\033','[','0','m'>;
    using ANSI_BOLD        = ct_string<'\033','[','1','m'>;
    using ANSI_UNDERLINE   = ct_string<'\033','[','4','m'>;
    using ANSI_RED         = ct_string<'\033','[','3','1','m'>;

    namespace DS {
        using ct_string_name = ct_string<'c','t','_','s','t','r','i','n','g',' ','-','>',' '>;
        using error_name     = ct_string<'e','r','r','o','r',' ','n','a','m','e'>;
        using complete_type  = ct_string<'c','o','m','p','l','e','t','e','_','t','y','p','e'>;
        using left_boundary  = ct_string<'|',' '>;
        using right_boundary = ct_string<' ','|'>;
        using colon          = ct_string<':'>;
    }

    template <typename Prefix, typename Str, typename Suffix>
    using WRAP_TEXT = typename ct_concat<Prefix, Str, Suffix>::type;
    template<typename Str>
    using PREFIX_NEW_LINE_TEXT = typename ct_concat<NEW_LINE, Str>::type;
    template<typename Str>
    using SUFFIX_NEW_LINE_TEXT = typename ct_concat<Str, NEW_LINE>::type;
    template<typename Str>
    using WRAPPED_NEW_LINE_TEXT = WRAP_TEXT<NEW_LINE, Str, NEW_LINE>;
    template <typename Color, typename Str, typename Tail = ANSI_RESET>
    using COLOR_TEXT = typename ct_concat<Color, Str, ANSI_RESET, Tail>::type;
    template <typename Color, typename Str, typename Tail = ANSI_RESET>
    using BOLD_COLOR_TEXT = typename ct_concat<
            ANSI_BOLD,
            COLOR_TEXT<Color, Str, Tail>
    >::type;
    template <typename Color, typename Str, typename Tail = ANSI_RESET>
    using UNDERLINE_COLOR_TEXT = typename ct_concat<
            ANSI_UNDERLINE,
            COLOR_TEXT<Color, Str, Tail>
    >::type;
    template <typename Color, typename Str, typename Tail = ANSI_RESET>
    using BOLD_UNDERLINE_COLOR_TEXT = typename ct_concat<
            ANSI_BOLD,
            UNDERLINE_COLOR_TEXT<Color, Str, Tail>
    >::type;

    template <char C, size_t... _>
    struct repeat_char_seq {
        using type = ct_string<(_, C)...>;
    };

    template <char C, size_t N>
    using repeat_char = typename repeat_char_seq<C, __integer_pack(N)...>::type;

    template <bool AddLastNewLine, typename... Strings>
    struct append_newline_each;

    template <bool AddLastNewLine, char... Cs>
    struct append_newline_each<AddLastNewLine, ct_string<Cs...>> {
        using type = typename ct_concat<
            ct_string<Cs...>,
            Util::conditional_t<AddLastNewLine, NEW_LINE, ct_string<>>
        >::type;
    };

    template <bool AddLastNewLine, char... Cs1, char... Cs2, typename... Rest>
    struct append_newline_each<AddLastNewLine, ct_string<Cs1...>, ct_string<Cs2...>, Rest...> {
        using type = typename ct_concat<
            ct_string<Cs1...>, NEW_LINE,
            typename append_newline_each<
                AddLastNewLine, ct_string<Cs2...>, Rest...
            >::type
        >::type;
    };

    template <bool AddLastNewLine, typename... Strings>
    using APPEND_NEWLINE_EACH = typename append_newline_each<AddLastNewLine, Strings...>::type;

    template <typename ErrorMsg, Util::size_t Length>
    using ERROR_TEXT = typename ct_concat<
        WRAPPED_NEW_LINE_TEXT<repeat_char<'-', Length>>,
        ErrorMsg,
        WRAPPED_NEW_LINE_TEXT<repeat_char<'-', Length>>
    >::type;

    template <typename Name>
    using Label_TEXT = typename ct_concat<
        Name,
        DS::colon
    >::type;

    template <typename Label, Util::size_t Max_Label_Length, typename Value, Util::size_t Max_Value_Length>
    using ENTRY_TEXT = typename ct_concat<
            DS::left_boundary,
            Label_TEXT<Label>,
            repeat_char<' ', (Max_Label_Length + ct_size<DS::colon>::size) - ct_size<Label_TEXT<Label>>::size>,
            Value,
            repeat_char<' ', Max_Value_Length - ct_size<Value>::size>,
            DS::right_boundary
        >::type;

    template <typename CTS>
    struct conversion_out;

    template <typename Label, typename Value>
    struct error_entry {
        using label = Label;
        using value = Value;
    };

    struct error_layout {
    private:
        template <size_t... Ns>
        static COMPILE_TIME Util::size_t max_of() {
            size_t arr[] = { Ns... };
            size_t m = 0;
            for (size_t v : arr) {
                if (v > m) m = v;
            }
            return m;
        }

    public:
        template <typename... Labels>
        static constexpr size_t max_label_length =
            max_of<ct_size<Labels>::size...>() + 2;

        template <typename... Values>
        static constexpr size_t max_value_length =
            max_of<ct_size<Values>::size...>();
    };

    template <typename... Entries>
    struct error_render {
    private:
        static constexpr Util::size_t max_label_len =
            error_layout::max_label_length<typename Entries::label...>;
        static constexpr Util::size_t max_value_len =
            error_layout::max_value_length<typename Entries::value...>;

        template <typename Entry>
        using entry_text = Util::ENTRY_TEXT<
            typename Entry::label,
            max_label_len,
            typename Entry::value,
            max_value_len
        >;

        using all_entries = Util::APPEND_NEWLINE_EACH<false, entry_text<Entries>...>;

        static constexpr size_t total_width =
            max_label_len + max_value_len
            + 2 * ct_size<Util::DS::left_boundary>::size
            + ct_size<Util::DS::colon>::size;

    public:
        using make = Util::BOLD_COLOR_TEXT<
            Util::ANSI_RED,
            Util::ERROR_TEXT<all_entries, total_width>
        >;
    };

    struct str_view {
        const char* ptr{};
        size_t length{};

        constexpr str_view() noexcept = default;
        constexpr str_view(const char* d, size_t s) noexcept : ptr(d), length(s) {}

        constexpr const char* data() const noexcept { return ptr; }
        constexpr Util::size_t size() const noexcept { return length; }
    };

    template<char... Cs>
    struct conversion_out<ct_string<Cs...>> {
    private:
        static constexpr size_t size = sizeof...(Cs);
        static constexpr char arr[size + 1] = { Cs..., '\0' };

    public:
        static COMPILE_TIME const char (&to_array())[size + 1] {
            return arr;
        }

        static COMPILE_TIME auto to_view() {
            return str_view{arr, size};
        }
    };

    struct conversion_in {

    };

    template <Util::size_t N>
    struct uint_to_cts;

    template <>
    struct uint_to_cts<0> {
        using type = ct_string<'0'>;
    };

    template <Util::size_t N>
    struct uint_to_cts {
    private:
        static constexpr Util::size_t q = N / 10;
        static constexpr Util::size_t r = N % 10;
    public:
        using type = typename ct_concat<
            typename uint_to_cts<q>::type,
            ct_string<'0' + r>
        >::type;
    };
};

template <char... Cs, Util::size_t I>
    REQUIRES(I >= ct_string<Cs...>::size)
struct ct_at<ct_string<Cs...>, I SPECIALIZATION_IMPL(I >= ct_string<Cs...>::size)> {
    using self = ct_string<Cs...>;
    static constexpr Util::size_t RequestedIndex = I;
    static constexpr Util::size_t Length = self::size;

    static_assert(
        RequestedIndex < Length,
#if __cplusplus < STD_VERSION_24
        "index_out_of_bounds"
#else
        Util::conversion_out<
            typename Util::error_render<
                Util::error_entry<Util::DS::error_name, CTS("index_out_of_bounds"_cts)>,
                Util::error_entry<Util::DS::complete_type, typename ct_concat<Util::DS::ct_string_name, self>::type>,
                Util::error_entry<CTS("RequestedIndex"_cts), typename Util::uint_to_cts<I>::type>,
                Util::error_entry<CTS("Length"_cts), typename Util::uint_to_cts<Length>::type>
            >::make
        >::to_view()
#endif
    );

    static constexpr char value{};
};

template <Util::size_t P, Util::size_t C, char... Cs, Util::size_t... Is>
    REQUIRES(P + C > sizeof...(Cs))
struct ct_slice<ct_string<Cs...>, P, C SPECIALIZATION_IMPL(P + C > sizeof...(Cs)), Is...> {
    using self = ct_string<Cs...>;
    static constexpr Util::size_t Start  = P;
    static constexpr Util::size_t Count  = C;
    static constexpr Util::size_t Length = self::size;

    static_assert(
        Start + Count <= Length,
#if __cplusplus < STD_VERSION_24
        "slice_range_out_of_bounds"
#else
        Util::conversion_out<
            typename Util::error_render<
                Util::error_entry<Util::DS::error_name, CTS("slice_range_out_of_bounds"_cts)>,
                Util::error_entry<Util::DS::complete_type, typename ct_concat<Util::DS::ct_string_name, self>::type>,
                Util::error_entry<CTS("Start"_cts), typename Util::uint_to_cts<Start>::type>,
                Util::error_entry<CTS("Count"_cts), typename Util::uint_to_cts<Count>::type>,
                Util::error_entry<CTS("Length"_cts), typename Util::uint_to_cts<Length>::type>
            >::make
        >::to_view()
#endif
    );

    using slice = ct_string<>;
};

template <char... Cs, Util::size_t I, char... SCs>
    REQUIRES(sizeof...(SCs) == 0 || (I + sizeof...(SCs) > sizeof...(Cs)))
struct ct_set<ct_string<Cs...>, I, ct_string<SCs...> SPECIALIZATION_IMPL(sizeof...(SCs) == 0 || (I + sizeof...(SCs) > sizeof...(Cs)))> {
    using self = ct_string<Cs...>;
    static constexpr Util::size_t Index  = I;
    static constexpr Util::size_t Count  = sizeof...(SCs);
    static constexpr Util::size_t Length = self::size;

#if __cplusplus < STD_VERSION_24
    static_assert(Count != 0, "set_empty_replacement");
    static_assert(Index + Count <= Length, "set_range_out_of_bounds");
#else
    consteval {
        auto error = []<typename ErrorName> {
            return Util::conversion_out<
                typename Util::error_render<
                    Util::error_entry<Util::DS::error_name, ErrorName>,
                    Util::error_entry<Util::DS::complete_type, typename ct_concat<Util::DS::ct_string_name, self>::type>,
                    Util::error_entry<CTS("Index"_cts), typename Util::uint_to_cts<Index>::type>,
                    Util::error_entry<CTS("ReplaceCount"_cts), typename Util::uint_to_cts<Count>::type>,
                    Util::error_entry<CTS("Length"_cts), typename Util::uint_to_cts<Length>::type>
                >::make
            >::to_view();
        };

        static_assert(Count != 0, error.template operator()<CTS("set_empty_replacement"_cts)>());
        static_assert(Index + Count <= Length, error.template operator()<CTS("set_range_out_of_bounds"_cts)>());
    }
#endif

    using type = ct_string<>;
};

template <char... Cs, Util::size_t P, Util::size_t C>
    REQUIRES(P + C > sizeof...(Cs))
struct ct_erase<ct_string<Cs...>, P, C SPECIALIZATION_IMPL(P + C > sizeof...(Cs))> {
    using self = ct_string<Cs...>;
    static constexpr Util::size_t Pos    = P;
    static constexpr Util::size_t Count  = C;
    static constexpr Util::size_t Length = self::size;

    static_assert(
        Pos + Count <= Length,
#if __cplusplus < STD_VERSION_24
        "erase_range_out_of_bounds"
#else
        Util::conversion_out<
            typename Util::error_render<
                Util::error_entry<Util::DS::error_name, CTS("erase_range_out_of_bounds"_cts)>,
                Util::error_entry<Util::DS::complete_type, typename ct_concat<Util::DS::ct_string_name, self>::type>,
                Util::error_entry<CTS("Pos"_cts), typename Util::uint_to_cts<Pos>::type>,
                Util::error_entry<CTS("Count"_cts), typename Util::uint_to_cts<Count>::type>,
                Util::error_entry<CTS("Length"_cts), typename Util::uint_to_cts<Length>::type>
            >::make
        >::to_view()
#endif
    );

    using type = ct_string<>;
};


template <char... Cs>
struct ct_string {
private:
    using self = ct_string<Cs...>;

public:
    static constexpr Util::size_t npos = static_cast<Util::size_t>(-1);

    static constexpr Util::size_t size = ct_size<self>::size;
    static constexpr bool empty = size == 0;

    template <Util::size_t I>
    static constexpr char at = ct_at<self, I>::value;

    template <typename... Others>
    static constexpr bool equal = ct_equal<self, Others...>::equal;

    template <typename Other>
    static constexpr int compare = ct_compare<self, Other>::value;

    template <typename... Others>
    using append = typename ct_concat<self, Others...>::type;

    template <typename... Others>
    using prepend = typename ct_concat<Others..., self>::type;

    template <Util::size_t P, Util::size_t C>
    using substr = typename ct_slice<self, P, C SPECIALIZATION_PARAMETER(void), __integer_pack(C)...>::slice;

    template <typename S>
    static constexpr Util::size_t find = ct_find<self, S>::pos;

    template <typename Other>
    static constexpr bool contains = find<Other> != npos;

    template <char C>
    using push_back = typename ct_concat<self, ct_string<C>>::type;

    template <char C>
    using push_front = typename ct_concat<ct_string<C>, self>::type;

    template <Util::size_t N = size CONSTRAINT(N > 0)
    using pop_back = typename ct_slice<self, 0, size - 1 SPECIALIZATION_PARAMETER(void), __integer_pack(N - 1)...>::slice;

    template <Util::size_t N = size CONSTRAINT(N > 0)
    using pop_front = typename ct_slice<self, 1, size - 1 SPECIALIZATION_PARAMETER(void), __integer_pack(N - 1)...>::slice;

    template <Util::size_t P, Util::size_t C>
    using erase = typename ct_erase<self, P, C>::type;

    template <Util::size_t I, typename Other>
    using set = typename ct_set<self, I, Other>::type;

    template <Util::size_t I, typename Other>
    using insert = typename ct_insert<self, I, Other>::type;

    template <typename F, typename T>
    using replace = typename ct_replace<self, F, T>::type;

    template <typename Other>
    static constexpr bool start_with = ct_start_with<self, Other>::value;

    template <typename Other>
    static constexpr bool end_with = ct_end_with<self, Other>::value;
};

}

#undef STD_VERSION_26
#undef STD_VERSION_24
#undef STD_VERSION_23
#undef STD_VERSION_20
#undef STD_VERSION_17

#undef COMPILE_TIME
#undef SPECIALIZATION
#undef SPECIALIZATION_PARAMETER
#undef SPECIALIZATION_IMPL
#undef CONSTRAINT
#undef REQUIRES