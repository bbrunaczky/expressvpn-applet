#include <config-test.h>

std::filesystem::path TestConfig::settingsFile() const
{
   return "./settings";
}

std::filesystem::path TestConfig::statFile() const
{
    return "./stat";
}


std::filesystem::path TestConfig::iconFile() const
{
    return "../resources/icon/icon.png";
}
