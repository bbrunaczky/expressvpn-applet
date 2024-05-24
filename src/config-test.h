#pragma once

#include <config.h>

class TestConfig: public Config
{

    std::filesystem::path settingsFile() const override;
    std::filesystem::path statFile() const override;
    std::filesystem::path iconFile() const override;
};
