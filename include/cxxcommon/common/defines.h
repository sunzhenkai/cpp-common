#pragma once

#include "list"
#include "mutex"
#include "shared_mutex"
#include "string"
#include "unordered_map"
#include "vector"

namespace cxxcommon {
typedef std::string Str;
using CStr = const std::string;
template <typename K, typename V> using Map = std::unordered_map<K, V>;
typedef Map<Str, Str> SSMap;
typedef std::list<Str> SList;
using Mutex = std::mutex;
using SMutex = std::shared_mutex;
using Lock = std::lock_guard<Mutex>;
using SLock = std::shared_lock<SMutex>;
using ULock = std::shared_lock<SMutex>;
typedef unsigned long int tid_t;
template <typename T> using Vector = std::vector<T>;
}  // namespace cxxcommon

#ifndef GET_OR_DEFAULT
#  define GET_OR_DEFAULT(m, k, dft) \
    m.find(k) != (m).end() ? (m).find(k)->second : dft
#endif  // GET_OR_DEFAULT

#ifndef MOVE
#  define MOVE(v) std::move(v)
#endif  // MOVE
