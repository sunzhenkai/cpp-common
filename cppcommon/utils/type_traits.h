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
}  // namespace cppcommon
