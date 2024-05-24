#pragma once

#include <memory>
#include <filesystem>
#include <mutex>

class Config
{
public:
    static void initialize(int argc, char **argv);
    static Config & instance();

    virtual ~Config();

    std::filesystem::path executable();
    uint16_t frequentlyUsed();
    
    virtual std::filesystem::path settingsFile() const = 0;
    virtual std::filesystem::path statFile() const = 0;
    virtual std::filesystem::path iconFile() const = 0;

private:

    void readSettings();
    
    static std::unique_ptr<Config> _instance;

    std::once_flag _settingsReadFlag;
    std::filesystem::path _executable;
    uint16_t _frequentlyUsed;

};
