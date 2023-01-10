#include "server.h"

#include "easylogging++.h"

namespace cloud
{
    Server::Server()
    {
    }

    Server::~Server()
    {
    }

    void Server::start()
    {
        LOG(INFO) << "Started";
    }

    void Server::stop()
    {
        LOG(INFO) << "Stopped";
    }
}