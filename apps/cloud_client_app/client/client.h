#define CLIENT_H
#ifdef CLIENT_H

#include <memory>

namespace cloud
{
    class ClientImpl;

    class Client
    {
    public:
        Client();
        ~Client();

        void start();
        void stop();

    private:
        std::unique_ptr<ClientImpl> client_impl_;
    };
}

#endif