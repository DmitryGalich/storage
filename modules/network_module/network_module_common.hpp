#pragma once

#include <string>
#include <functional>

namespace network_module
{
    typedef std::string Url;
    typedef std::function<std::string()> Callback;

    struct Urls
    {
        static const Url kPageNotFound_;
    };
}