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
        THETVDB_USERKEY,

        // Filesystem informations
        PATH_TO_COPY_ROOT,
        EXCLUDED_FOLDERS,
        SEASON_DICT_LOCATION

    };

    void UseDefaultConfiguration() const;

    std::wstring operator[]( ConfigOption key ) const;

private:
    File::CINIFile Config;

    static const std::unordered_map<ConfigOption, std::wstring> OptionToString;
};

