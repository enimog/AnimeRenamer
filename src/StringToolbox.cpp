#include "stdafx.h"
#include "StringToolbox.h"
#include <locale>
#include <codecvt>

namespace Toolbox
{
    namespace StringToolbox
    {
        std::wstring StringToWide( std::string const & string )
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            return converter.from_bytes( string );
        }

        std::string WideToString( std::wstring const & string )
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            return converter.to_bytes( string );
        }

        std::vector<std::wstring> Split( std::wstring const & string, std::wstring const & delimiter )
        {
            std::vector<std::wstring> result;
            auto temp( string );
            size_t pos = 0;

            while ((pos = temp.find( delimiter )) != std::wstring::npos)
            {
                result.push_back(temp.substr( 0, pos ));
                temp.erase( 0, pos + delimiter.length() );
            }
            result.push_back( temp );

            return result;
        }

        std::vector<std::string> Split( std::string const & string, std::string const & delimiter )
        {
            std::vector<std::string> result;
            auto temp( string );
            size_t pos = 0;

            while ((pos = temp.find( delimiter )) != std::string::npos)
            {
                result.push_back( temp.substr( 0, pos ) );
                temp.erase( 0, pos + delimiter.length() );
            }
            result.push_back( temp );

            return result;
        }

        std::vector<std::wstring> Split( std::wstring const & string, wchar_t const & delimiter )
        {
            std::vector<std::wstring> result;
            auto temp( string );
            size_t pos = 0;

            while ((pos = temp.find( delimiter )) != std::wstring::npos)
            {
                result.push_back( temp.substr( 0, pos ) );
                temp.erase( 0, pos + 1 );
            }
            result.push_back( temp );

            return result;
        }

        std::vector<std::string> Split( std::string const & string, char const & delimiter )
        {
            std::vector<std::string> result;
            auto temp( string );
            size_t pos = 0;

            while ((pos = temp.find( delimiter )) != std::string::npos)
            {
                result.push_back( temp.substr( 0, pos ) );
                temp.erase( 0, pos + 1 );
            }
            result.push_back( temp );

            return result;
        }

        std::wstring replace(std::wstring const& str, const std::wstring& from, const std::wstring& to)
        {
            auto strcopy = str;
            size_t start_pos = str.find(from);
            if(start_pos == std::wstring::npos)
                return str;
            strcopy.replace(start_pos, from.length(), to);
            return strcopy;
        }
    }
}
