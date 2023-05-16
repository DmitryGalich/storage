#pragma once

#include <string>
#include <functional>

namespace network_module
{
    typedef std::string Url;
    typedef std::function<std::string()> HttpCallback;

    namespace web_sockets
    {
        typedef std::function<void(const std::string &)> InputCallback;
        typedef std::function<std::string()> OutputCallback;
        typedef std::function<void()> OutputClientCallback;
    }

    struct Urls
    {
        static const Url kPageNotFound_;
    };
}