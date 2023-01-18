#include "client.h"

#include "easylogging++.h"

namespace cloud
{
    Client::Client()
    {
        LOG(DEBUG) << "Constructor";
    }

    Client::~Client()
    {
        LOG(DEBUG) << "Destructor";
    }

    void Client::start()
    {
        LOG(INFO) << "Started";
    }

    void Client::stop()
    {
        LOG(INFO) << "Stopped";
    }
}