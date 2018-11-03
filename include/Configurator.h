#pragma once
#include "INIFile.h"
#include <unordered_map>

class CConfigurator
{
public:
    CConfigurator();
    ~CConfigurator();

    enum ConfigOption
    {
        // TVDB authentication
        THETVDB_API_KEY,
        THETVDB_USERNAME,
        THETVDB_USERKEY

    };

    void UseDefaultConfiguration() const;

    std::wstring operator[]( ConfigOption key ) const;

private:
    File::CINIFile Config;

    static const std::unordered_map<ConfigOption, std::wstring> OptionToString;
};

