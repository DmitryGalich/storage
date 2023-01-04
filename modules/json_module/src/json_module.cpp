#include "json_module.h"

#include <iostream>

#include "json.hpp"

namespace json_module
{
    void make_fun()
    {
        nlohmann::json json_object = {
            {"user_id", 123},
            {"user_name", "Dmitry"}};

        std::cout << json_object << std::endl;
    }
}
