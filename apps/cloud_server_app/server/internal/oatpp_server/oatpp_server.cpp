#include "oatpp_server.h"

#include "easylogging++.h"

#include "../abstract_server.h"

#include "oatpp/core/base/Environment.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/web/protocol/http/outgoing/Response.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

namespace
{
#include OATPP_CODEGEN_BEGIN(DTO)

    class StatusDto : public oatpp::DTO
    {
        DTO_INIT(StatusDto, DTO)

        DTO_FIELD_INFO(status)
        {
            info->description = "Short status text";
        }
        DTO_FIELD(String, status);

        DTO_FIELD_INFO(code)
        {
            info->description = "Status code";
        }
        DTO_FIELD(Int32, code);

        DTO_FIELD_INFO(message)
        {
            info->description = "Verbose message";
        }
        DTO_FIELD(String, message);
    };

#include OATPP_CODEGEN_END(DTO)
}

namespace
{
    class ErrorHandler : public oatpp::web::server::handler::ErrorHandler
    {
    public:
        ErrorHandler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper> &object_mapper)
            : object_mapper_(object_mapper) {}

        std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>
        handleError(const oatpp::web::protocol::http::Status &status,
                    const oatpp::String &message,
                    const Headers &headers) override
        {
            auto error = StatusDto::createShared();
            error->status = "ERROR";
            error->code = status.code;
            error->message = message;

            auto response = oatpp::web::protocol::http::outgoing::ResponseFactory::createResponse(status, error, object_mapper_);

            for (const auto &pair : headers.getAll())
            {
                response->putHeader(pair.first.toString(), pair.second.toString());
            }

            return response;
        }

    private:
        std::shared_ptr<oatpp::data::mapping::ObjectMapper> object_mapper_;
    };
}

namespace cloud
{
    namespace internal
    {
        class OatppServer::OatppServerImpl
        {
        public:
            OatppServerImpl() = delete;
            OatppServerImpl(const ServerConfig &config);
            ~OatppServerImpl() = default;

            bool start();
            void stop();

        private:
            bool init();
            bool run();

        private:
            const ServerConfig kConfig_;

            std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper> object_mapper_;
            std::shared_ptr<oatpp::network::tcp::server::ConnectionProvider> connection_provider_;
            std::shared_ptr<oatpp::web::server::HttpRouter> router_;
            std::shared_ptr<oatpp::web::server::HttpConnectionHandler> connection_handler_;
        };

        OatppServer::OatppServerImpl::OatppServerImpl(const ServerConfig &config) : kConfig_(config) {}

        bool OatppServer::OatppServerImpl::init()
        {
            oatpp::base::Environment::init();

            object_mapper_.reset();
            object_mapper_ = oatpp::parser::json::mapping::ObjectMapper::createShared();
            if (!object_mapper_)
            {
                LOG(ERROR) << "ObjectMapper is not created";
                return false;
            }

            connection_provider_.reset();
            connection_provider_ = oatpp::network::tcp::server::ConnectionProvider::createShared(
                {kConfig_.host_,
                 static_cast<v_uint16>(kConfig_.port_),
                 (kConfig_.is_ip_v6_family_ ? oatpp::network::Address::IP_6 : oatpp::network::Address::IP_4)});
            if (!connection_provider_)
            {
                LOG(ERROR) << "ConnectionProvider is not created";
                return false;
            }

            router_.reset();
            router_ = oatpp::web::server::HttpRouter::createShared();
            if (!router_)
            {
                LOG(ERROR) << "HttpRouter is not created";
                return false;
            }

            connection_handler_.reset();
            connection_handler_ = oatpp::web::server::HttpConnectionHandler::createShared(router_);
            connection_handler_->setErrorHandler(std::make_shared<ErrorHandler>(object_mapper_));
            if (!connection_handler_)
            {
                LOG(ERROR) << "HttpConnectionHandler is not created";
                return false;
            }

            return true;
        }

        bool OatppServer::OatppServerImpl::run()
        {

            return true;
        }

        bool OatppServer::OatppServerImpl::start()
        {
            if (!init())
                return false;

            if (!run())
                return false;

            return true;
        }

        void OatppServer::OatppServerImpl::stop()
        {
            connection_handler_.reset();
            router_.reset();
            connection_provider_.reset();
            object_mapper_.reset();

            oatpp::base::Environment::destroy();
        }
    }
}

namespace cloud
{
    namespace internal
    {
        OatppServer::OatppServer(const ServerConfig &config) : server_impl_(std::make_unique<OatppServer::OatppServerImpl>(config))
        {
        }
        OatppServer::~OatppServer()
        {
        }

        bool OatppServer::start()
        {
            if (!server_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            return server_impl_->start();
        }
        void OatppServer::stop()
        {
            if (!server_impl_)
            {
                LOG(ERROR) << "Implementation is not created";
                return;
            }

            server_impl_->stop();
        }
    }
}