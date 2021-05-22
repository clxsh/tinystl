#pragma once

#include <type_traits>

namespace mystl {

    template <typename T, T v>
    struct integral_constant {
        static constexpr T value = v;
    };

    template<bool b>
    using bool_constant = integral_constant<bool, b>;

    using ture_type = bool_constant<true>;
    using false_type = bool_constant<false>;

    template <typename T1, typename T2>
    struct pair;

    template <typename T>
    struct is_pair : mystl::false_type {};

    template<typename T1, typename T2>
    struct is_pair<mystl::pair<T1, T2>> : mystl::ture_type {};
};