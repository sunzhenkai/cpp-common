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
/// Basic
template <typename T>
struct unwrap_type {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T>
using unwrap_type_t = typename unwrap_type<T>::type;

/// STL
template <typename, typename = void>
struct has_iterator : std::false_type {};

template <typename T>
struct has_iterator<T, std::void_t<typename T::iterator>> : std::true_type {};

template <typename, typename = void>
struct has_const_iterator : std::false_type {};

template <typename T>
struct has_const_iterator<T, std::void_t<typename T::const_iterator>> : std::true_type {};

template <typename T, typename = void>
struct has_begin_end : std::false_type {};
template <typename T>
struct has_begin_end<
    T, std::void_t<decltype(std::declval<const T &>().begin()), decltype(std::declval<const T &>().end())>>
    : std::true_type {};

// vector
template <typename T>
struct is_vector_impl : std::false_type {};

template <typename T, typename Alloc>
struct is_vector_impl<std::vector<T, Alloc>> : std::true_type {};

template <typename T>
struct is_vector : is_vector_impl<unwrap_type_t<T>> {};

template <typename T>
inline constexpr bool is_vector_v = is_vector_impl<unwrap_type_t<T>>::value;

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

// array
template <class T>
struct is_array_impl : std::is_array<T> {};

template <class T, std::size_t N>
struct is_array_impl<std::array<T, N>> : std::true_type {};

template <typename T>
struct is_array : is_array_impl<unwrap_type_t<T>> {};

template <typename... Args>
inline constexpr bool is_array_v = is_array_impl<unwrap_type_t<Args>...>::value;

// map
template <typename T, typename = void>
struct is_map_impl : std::false_type {};

template <typename T>
struct is_map_impl<T, std::void_t<typename T::mapped_type>> : std::true_type {};

template <typename T>
struct is_map : is_map_impl<unwrap_type_t<T>> {};

template <typename T>
inline constexpr bool is_map_v = is_map_impl<unwrap_type_t<T>>::value;

template <typename T>
struct is_container_impl : std::conjunction<has_const_iterator<T>, has_begin_end<T>> {};

// exclude string
template <typename T>
struct is_container : std::conjunction<is_container_impl<unwrap_type_t<T>>,
                                       std::negation<std::is_same<std::decay_t<unwrap_type_t<T>>, std::string>>> {};

template <typename T>
inline constexpr bool is_container_v = is_container<unwrap_type_t<T>>::value;

// basic types or string
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
constexpr bool is_string_like_v = is_string_like<unwrap_type_t<T>>::value;
}  // namespace cppcommon
