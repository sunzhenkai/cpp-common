#pragma once
#include "curl/curl.h"
#include "cxxcommon/common/defines.h"

namespace cxxcommon::net {
    Str URLEncode(CStr &data);
}