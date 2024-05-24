#pragma once

#include <config.h>
#include <filesystem>

class ProdConfig: public Config
{
public:
    std::filesystem::path settingsFile() const override;
    std::filesystem::path statFile() const override;
    std::filesystem::path iconFile() const override;

private:
    std::string getEnvVar(char const * key) const;
    std::filesystem::path home() const;
    std::filesystem::path configFolder() const;
};
