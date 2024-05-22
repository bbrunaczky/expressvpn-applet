#pragma once

#include <config.h>

class TestConfig: public Config
{
    std::string statFile() const override;
    std::string icon() const override;
};
