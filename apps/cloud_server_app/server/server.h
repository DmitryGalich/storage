#define SERVER_H
#ifdef SERVER_H

namespace cloud
{
    class Server
    {
    public:
        Server();
        ~Server();

        void start();
        void stop();
    };
}

#endif