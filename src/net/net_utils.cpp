#include "cxxcommon/net/net_utils.h"

namespace cxxcommon::net {
    Str URLEncode(const Str &data) {
        Str result;
        CURL *curl = curl_easy_init();
        char *output = nullptr;
        if (curl) {
            output = curl_easy_escape(curl, data.c_str(), data.length());
        }
        if (output) {
            result = output;
            curl_free(output);
        }
        curl_easy_cleanup(curl);
        return result;
    }

    Str URLDecode(const Str &data) {
        Str result;
        CURL *curl = curl_easy_init();
        char *output = nullptr;
        if (curl) {
            output = curl_easy_unescape(curl, data.c_str(), data.length(), nullptr);
        }
        if (output) {
            result = output;
            curl_free(output);
        }
        curl_easy_cleanup(curl);
        return result;
    }
}