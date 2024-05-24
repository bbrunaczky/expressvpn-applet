#include <config.h>
#include <config-test.h>
#include <config-prod.h>

#include <iostream>
#include <mutex>
#include <boost/program_options.hpp>
#include <fstream>

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


std::filesystem::path Config::executable()
{
    std::call_once(_settingsReadFlag, [this] () { readSettings(); });
    return _executable;
}


uint16_t Config::frequentlyUsed()
{
    std::call_once(_settingsReadFlag, [this] () { readSettings(); });
    return _frequentlyUsed;
}


void Config::readSettings()
{
    std::filesystem::path defaultExecutable{"/usr/bin/expressvpn"};
    uint16_t defaultFrequentlyUsed{3};

    namespace po = boost::program_options;
    po::options_description desc("Settings");
    desc.add_options()
        ("executable", po::value<std::filesystem::path>(&_executable)->default_value(defaultExecutable), "ExpressVPN executable")
        ("frequently_used", po::value<uint16_t>(&_frequentlyUsed)->default_value(defaultFrequentlyUsed), "Number of items in the Frequently used section ");


    if (std::filesystem::exists(settingsFile()))
    {
        po::variables_map vm;
        po::store(parse_config_file(settingsFile().string().c_str(), desc), vm);
        po::notify(vm);
    }
    else
    {
        std::fstream out(settingsFile(), std::ios::out);
        out << "executable" << "=" << defaultExecutable.string() << std::endl;
        out << "frequently_used" << "=" << defaultFrequentlyUsed << std::endl;
        out.close();

        _executable = defaultExecutable;
        _frequentlyUsed = defaultFrequentlyUsed;
    }
}
