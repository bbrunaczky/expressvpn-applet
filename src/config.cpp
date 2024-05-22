#include <config.h>
#include <config-test.h>
#include <config-prod.h>

#include <iostream>
#include <mutex>


std::unique_ptr<Config> Config::_instance;

void Config::initialize(int argc, char ** argv)
{
    static std::once_flag flag;
    std::call_once(flag,
                   [&argc, &argv] ()
                   {
                       if (2 == argc && std::string("--test") == argv[1])
                       {
                           std::cout << "Test mode" << std::endl;
                           Config::_instance = std::make_unique<TestConfig>();
                       }
                       else
                       {
                           Config::_instance = std::make_unique<ProdConfig>();
                       }
                   });
}


Config & Config::instance()
{
    if (!_instance)
    {
        throw std::runtime_error("Config must be initialized first!");
    }
    return *_instance;
}


Config::~Config()
{
}
