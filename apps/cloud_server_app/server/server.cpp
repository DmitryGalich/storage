#include "server.h"

#include "easylogging++.h"

#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"

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