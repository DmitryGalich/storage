#include "server.h"

#include "easylogging++.h"

namespace cloud
{
    Server::Server()
    {
        LOG(DEBUG) << "Constructor";
    }

    Server::~Server()
    {
        LOG(DEBUG) << "Destructor";
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