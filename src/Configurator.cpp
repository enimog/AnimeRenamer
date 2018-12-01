#include "stdafx.h"
#include "Configurator.h"
#include "FileToolbox.h"

const std::unordered_map<CConfigurator::ConfigOption, std::wstring> CConfigurator::OptionToString = {
    { THETVDB_API_KEY , L"ApiKey" },
    { THETVDB_USERNAME, L"Username_TheTVDB" },
    { THETVDB_USERKEY ,  L"Userkey" },
    { ANIDB_USERNAME , L"Username_AniDB" },
    { ANIDB_PASSWORD , L"Password" },
    { PATH_TO_COPY_ROOT , L"PathToCopyRoot" },
    { EXCLUDED_FOLDERS , L"ExcludedFolders" },
    { SEASON_DICT_LOCATION , L"seasonDictLocation" }
};

bool UsedDefaultConfig = false;

CConfigurator::CConfigurator() : Config( L"config.ini" )
{
    /*if (Toolbox::FileToolbox::Exists( L"config.ini" ))
    {
        Validate();
        LOG_INFO( "Using the loaded configuration" );
    }
    else
    {
        LOG_INFO( "Using the default configuration" );
        UseDefaultConfiguration();
    }*/
    if (!UsedDefaultConfig)
    {
        UseDefaultConfiguration();
    }
}


CConfigurator::~CConfigurator()
{
}

void CConfigurator::UseDefaultConfiguration() const
{
    LOG_TRACE("Begin creating the default configuration");

    // Get all values
    const auto api_key = (*this)[THETVDB_API_KEY];
    const auto username_thetvdb = (*this)[THETVDB_USERNAME];
    const auto userkey = (*this)[THETVDB_USERKEY];
    const auto username_anidb = (*this)[ANIDB_USERNAME];
    const auto password = (*this)[ANIDB_PASSWORD];
    const auto copypath = (*this)[PATH_TO_COPY_ROOT];
    const auto excludedFolders = (*this)[EXCLUDED_FOLDERS];
    const auto dictLocation = (*this)[SEASON_DICT_LOCATION];

    // Clear the configuration if it existed
    Config.clear();

    // Set here user specific configuration
    Config.AppendSection( L"TheTVDB Authentication" );
    Config.AppendKeyValue( L"ApiKey", api_key );
    Config.AppendKeyValue( L"Username_TheTVDB", username_thetvdb );
    Config.AppendKeyValue( L"Userkey", userkey );
    Config.AppendEmptyLine();

    Config.AppendSection( L"AniDB Authentication" );
    Config.AppendKeyValue( L"Username_AniDB", username_anidb );
    Config.AppendKeyValue( L"Password", password );
    Config.AppendEmptyLine();

    Config.AppendSection( L"General" );
    Config.AppendKeyValue( L"PathToCopyRoot", copypath );
    Config.AppendKeyValue( L"ExcludedFolders", excludedFolders );
    Config.AppendKeyValue( L"seasonDictLocation", dictLocation );

    UsedDefaultConfig = true;
    LOG_TRACE( "End creating the default configuration" );
}

std::wstring CConfigurator::operator[]( ConfigOption key ) const
{
    auto iter = OptionToString.find( key );
    if (iter != OptionToString.end())
    {
        return Config[iter->second];
    }

    return std::wstring();
}
