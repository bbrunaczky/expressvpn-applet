#pragma once

#include <memory>

class Config
{
public:
    static void initialize(int argc, char **argv);
    static Config & instance();

    virtual ~Config();

    virtual std::string statFile() const = 0;
    virtual std::string icon() const = 0;

private:
    static std::unique_ptr<Config> _instance;
};
