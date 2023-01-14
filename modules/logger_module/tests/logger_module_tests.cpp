#include "configs/cmake_config.h"

#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

void configure_logger()
{
    el::Configurations conf(CMAKE_CURRENT_SOURCE_DIR + std::string{"/tests/configs/log_config.conf"});
    el::Loggers::reconfigureAllLoggers(conf);
}

class BusinessLogicModule
{

public:
    BusinessLogicModule()
    {
        LOG(DEBUG) << "Constructor";
    }
    ~BusinessLogicModule()
    {
        LOG(DEBUG) << "Destructor";
    }

    void run_business_logic()
    {
        LOG(INFO) << "running business logic";
    }
};

int main()
{
    configure_logger();

    LOG(INFO) << "================================";
    LOG(INFO) << PROJECT_NAME;
    LOG(INFO) << "version " << PROJECT_VERSION;

    BusinessLogicModule module;
    module.run_business_logic();

    LOG(INFO) << "================================";

    return 0;
}