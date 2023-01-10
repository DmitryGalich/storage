#define CRC_MODULE
#ifdef CRC_MODULE

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