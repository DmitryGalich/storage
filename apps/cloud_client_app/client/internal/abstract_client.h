#define ABSTRACT_CLIENT_H
#ifdef ABSTRACT_CLIENT_H

namespace cloud
{
    namespace internal
    {
        class AbstractClient
        {
        public:
            virtual ~AbstractClient() = 0;

            virtual void start() = 0;
            virtual void stop() = 0;
        };
    }
}

#endif