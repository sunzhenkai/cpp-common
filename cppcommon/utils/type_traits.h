/**
 * @file type_traits.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-04-25 19:23:12
 */
#pragma once
#include <type_traits>
#include <vector>

namespace cppcommon {
template <typename T>
struct is_vector : std::false_type {};

template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type {};

template <typename T>
struct VectorElementType {
  using type = void;
};

template <typename V, typename Allocator>
struct VectorElementType<std::vector<V, Allocator>> {
  using type = V;
};

template <typename T>
using VectorElementType_t = typename VectorElementType<T>::type;

template <typename T>
struct unwrap_type {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T>
using unwrap_type_t = typename unwrap_type<T>::type;

template <typename T>
struct is_basic_or_string_like
    : std::disjunction<std::is_arithmetic<unwrap_type_t<T>>, std::is_same<unwrap_type_t<T>, std::string>,
                       std::is_same<unwrap_type_t<T>, std::string_view>, std::is_same<unwrap_type_t<T>, char *>> {};

template <typename T>
constexpr bool is_basic_or_string_like_v = is_basic_or_string_like<T>::value;

template <typename T>
struct is_string_like
    : std::disjunction<std::is_same<unwrap_type_t<T>, std::string>, std::is_same<unwrap_type_t<T>, std::string_view>,
                       std::is_same<unwrap_type_t<T>, char *>> {};

template <typename T>
constexpr bool is_string_like_v = is_string_like<T>::value;
}  // namespace cppcommon
