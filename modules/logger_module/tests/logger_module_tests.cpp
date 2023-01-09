#include <iostream>

// #include "../../../build/third_party/easylogging/easyloggingpp_src/src/easylogging++.h"

#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

int main()
{
    LOG(INFO) << "My first info log using default logger111111111111111";

    return 0;
}