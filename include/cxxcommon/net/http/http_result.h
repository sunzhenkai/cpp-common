#pragma once

#include "cxxcommon/common/defines.h"

namespace cxxcommon {
    class HTTPResult {
    public:
        long code;
        Str content;
        SSMap headers;
    };
}
