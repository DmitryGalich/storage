#include "server.h"

#include "easylogging++.h"

#include "oatpp/network/Server.hpp"

namespace cloud
{
    Server::Server()
    {
        LOG(DEBUG) << "Constructor";

        // oatpp::network::Server server;
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