#pragma once

#include "cxxcommon/defines.h"

namespace cxxcommon {
    class HTTPResult {
    public:
        long code;
        Str content;
        SSMap headers;
    };
}
