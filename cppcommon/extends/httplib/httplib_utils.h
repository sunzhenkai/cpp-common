/**
 * @file httplib.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-07-19 12:45:43
 */
#pragma once
#include <iostream>
#include <optional>
#include <string>

#include "cppcommon/3rd/httplib/httplib.h"

namespace cppcommon {
inline std::optional<std::string> HttpGet(const std::string& host, const std::string& path,
                                          const httplib::Headers& headers = httplib::Headers()) {
  httplib::Client cli(host);
  cli.set_connection_timeout(std::chrono::seconds(3));
  cli.set_read_timeout(std::chrono::seconds(5));

  if (auto res = cli.Get(path.c_str(), headers)) {
    if (res->status == 200) {
      return res->body;
    } else {
      // std::cerr << "HTTP GET failed for " << host << path << " with status: " << res->status << std::endl;
      return std::nullopt;
    }
  } else {
    auto err = res.error();
    // std::cerr << "HTTP GET request failed for " << host << path << ": " << httplib::to_string(err) << std::endl;
    return std::nullopt;
  }
}
}  // namespace cppcommon
