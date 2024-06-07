#ifndef RTCFG_HTTP_CLIENT_H
#define RTCFG_HTTP_CLIENT_H

#include "pthread.h"
#include "curl/curl.h"
#include "cxxcommon/defines.h"
#include "cxxcommon/net/http/http_result.h"
#include "utils/url.h"

namespace cxxcommon {
#define CURL_ERR(...) curl_easy_strerror(__VA_ARGS__)

    class HTTPClient {
    private:
        static SSMap EMPTY_MAP;
        pthread_key_t pthread_key_{};

    public:
        HTTPClient();
        HTTPResult
        Get(const Str &path, const SSMap &params = EMPTY_MAP, const SSMap &headers = EMPTY_MAP, long timeout = 5000);
        static void InitCurl();
        static void CleanUp();

    private:
        CURL *GetCurlHandler() const;
        static void DestroyCurlHandler(void *arg);
        static Str EncodingParams(const SSMap &params);
        static SList AssembleHeaders(const SSMap &headers);
        static void SetBasicOpts(CURL *curl_handler);
    };
}

#endif //RTCFG_HTTP_CLIENT_H
