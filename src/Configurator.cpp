#include "stdafx.h"
#include "Configurator.h"
#include "FileToolbox.h"

const std::unordered_map<CConfigurator::ConfigOption, std::wstring> CConfigurator::OptionToString = {
    { THETVDB_API_KEY , L"ApiKey" },
    { THETVDB_USERNAME, L"Username" },
    { THETVDB_USERKEY ,  L"Userkey" }
};

CConfigurator::CConfigurator() : Config( L"config.ini" )
{
    if (Toolbox::FileToolbox::Exists( L"config.ini" ))
    {
        LOG_INFO( "Using the loaded configuration" );
    }
    else
    {
        LOG_INFO( "Using the default configuration" );
        UseDefaultConfiguration();
    }
}


CConfigurator::~CConfigurator()
{
}

void CConfigurator::UseDefaultConfiguration() const
{
    LOG_TRACE("Begin creating the default configuration");

    // Clear the configuration if it existed
    Config.clear();

    // Set here user specific configuration
    Config.AppendSection( L"Authentication" );
    Config.AppendKeyValue( L"ApiKey", L"" );
    Config.AppendKeyValue( L"Username", L"" );
    Config.AppendKeyValue( L"Userkey", L"" );

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