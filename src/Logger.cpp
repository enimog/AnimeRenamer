#include "stdafx.h"
#include "Logger.h"
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include "Constants.h"
#include "StringToolbox.h"
#include "FileToolbox.h"
#include <stddef.h>

static log4cplus::ConfigureAndWatchThread* configurator = nullptr;

CLogger::CLogger() : initializer()
{
}


CLogger::~CLogger()
{
    if (configurator != nullptr)
    {
        delete configurator;
        configurator = nullptr;
    }

    log4cplus::threadCleanup();
}

void CLogger::log(std::string const& context, size_t const line, std::wstring const & text, const log4cplus::LogLevel ll)
{
    auto path( Toolbox::FileToolbox::ToAbsolutePath( CONST_LOG_FILE_NAME ) );

    if (configurator == nullptr && Toolbox::FileToolbox::Exists( path ))
    {
        configurator = new log4cplus::ConfigureAndWatchThread( path );
    }

    if (configurator != nullptr)
    {
        auto logger = log4cplus::Logger::getInstance(Toolbox::StringToolbox::StringToWide( context ) );
        auto format = L"Line number " + std::to_wstring( line ) + L" : " + text;

        switch (ll)
        {
        case log4cplus::FATAL_LOG_LEVEL:
            LOG4CPLUS_FATAL( logger, format );
            break;
        case log4cplus::ERROR_LOG_LEVEL:
            LOG4CPLUS_ERROR( logger, format );
            break;
        case log4cplus::WARN_LOG_LEVEL:
            LOG4CPLUS_WARN( logger, format );
            break;
        case log4cplus::INFO_LOG_LEVEL:
            LOG4CPLUS_INFO( logger, format );
            break;
        case log4cplus::DEBUG_LOG_LEVEL:
            LOG4CPLUS_DEBUG( logger, format );
            break;
        case log4cplus::TRACE_LOG_LEVEL:
            LOG4CPLUS_TRACE( logger, format );
            break;
        default:
            break;
        }
    }
}

void CLogger::log( std::string const& context, size_t const line, std::string const & text, const log4cplus::LogLevel ll )
{
    log(context, line, Toolbox::StringToolbox::StringToWide(text), ll );
}
