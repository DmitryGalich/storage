#pragma once

#include <string>
#include <functional>

namespace network_module
{
    typedef std::string Url;
    typedef std::function<std::string()> HttpCallback;

    namespace web_sockets
    {
        typedef std::function<void(const std::string &)> ReadingCallback;
        typedef std::function<std::string()> WritingCallback;
    }

    struct Urls
    {
        static const Url kPageNotFound_;
    };
}