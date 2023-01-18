#define CLIENT_H
#ifdef CLIENT_H

namespace cloud
{
    class Client
    {
    public:
        Client();
        ~Client();

        void start();
        void stop();
    };
}

#endif