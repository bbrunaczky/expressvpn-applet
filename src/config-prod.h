#pragma once

#include <config.h>
#include <filesystem>

class ProdConfig: public Config
{
public:
    std::string statFile() const override;
    std::string icon() const override;

private:
    std::string getEnvVar(char const * key) const;
    std::filesystem::path home() const;
    std::filesystem::path configFolder() const;
};
