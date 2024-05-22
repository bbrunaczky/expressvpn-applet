#include <config-prod.h>
#include <resource.h>

#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <string>


std::string ProdConfig::statFile() const
{
    return configFolder() / "stat";
}


std::string ProdConfig::icon() const
{
    return resource::shareFolder / "icon.png";
}


std::string ProdConfig::getEnvVar(char const * key) const
{
    char const * value = std::getenv(key);
    return nullptr == value ? std::string() : std::string(value);
}


std::filesystem::path ProdConfig::home() const
{
    std::string value{getEnvVar("HOME")};
    if (value.empty())
    {
        throw std::runtime_error("HOME environment variable is not set!");
    }
    return value;
}


std::filesystem::path ProdConfig::configFolder() const
{
    std::filesystem::path path{getEnvVar("XDG_CONFIG_HOME")};
    if (path.empty())
    {
        path = home() / ".config";
    }
    path /= "expressvpn-applet";

    if (!std::filesystem::exists(path))
    {
        std::filesystem::create_directories(path);
    }
    return path;
}

