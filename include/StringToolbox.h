#pragma once

#include <memory>
#include <vector>
#include <algorithm>

namespace Toolbox
{
    namespace StringToolbox
    {
        std::wstring StringToWide( std::string const& string );

        std::string WideToString( std::wstring const& string );

        template<class T>
        std::vector<std::basic_string<T>> Split( std::basic_string<T> const & str, std::basic_string<T> const & delimiter )
        {
            std::vector<std::basic_string<T>> result;
            auto temp( str );
            size_t pos = 0;

            while ((pos = temp.find( delimiter )) != std::basic_string<T>::npos)
            {
                result.push_back( temp.substr( 0, pos ) );
                temp.erase( 0, pos + delimiter.length() );
            }
            result.push_back( temp );

            return result;
        }

        template<class T>
        std::vector<std::basic_string<T>> Split( std::basic_string<T> const & str, T const & delimiter )
        {
            std::vector<std::basic_string<T>> result;
            auto temp( str );
            size_t pos = 0;

            while ((pos = temp.find( delimiter )) != std::basic_string<T>::npos)
            {
                result.push_back( temp.substr( 0, pos ) );
                temp.erase( 0, pos + 1 );
            }
            result.push_back( temp );

            return result;
        }

        template<class T>
        std::vector<std::basic_string<T>> Split( std::basic_string<T> const & str, const T* delimiter )
        {
            return Split(str, std::basic_string<T>(delimiter));
        }

        template<class T>
        std::basic_string<T> replace(std::basic_string<T> const& str, std::basic_string<T> const& from, std::basic_string<T> const& to)
        {
            auto strcopy = str;
            size_t start_pos = str.find(from);
            if(start_pos == std::basic_string<T>::npos)
                return str;
            strcopy.replace(start_pos, from.length(), to);
            return strcopy;
        }

        template<class T>
        std::basic_string<T> replace(std::basic_string<T> const& str, const T* from, const T* to)
        {
            return replace(str, std::basic_string<T>(from), std::basic_string<T>(to));
        }

        template<typename ... Args>
        std::wstring Format( const std::wstring& format, Args ... args )
        {
            if (sizeof...(Args) > 0)
            {
                wchar_t buffer[1000];
                std::swprintf( buffer, 1000, StringToolbox::replace( format, L"%s", L"%ls" ).c_str(), args ... );
                return std::wstring( buffer );
            }
            return format;
        }

        template<typename ... Args>
        std::string Format( const std::string& format, Args ... args )
        {
            if (sizeof...(Args) > 0)
            {
                size_t size = sprintf( nullptr, format.c_str(), args ... ) + 1;
                std::unique_ptr<char[]> buf( new char[size] );
                sprintf( buf.get(), format.c_str(), args ... );
                return std::string( buf.get(), buf.get() + size - 1 );
            }
            return format;
        }

        // trim from start (in place)
        template<class T>
        std::basic_string<T> ltrim(std::basic_string<T> const& s)
        {
            std::basic_string<T> ret = s;
            ret.erase(ret.begin(), std::find_if(ret.begin(), ret.end(), [](int ch) {
                return !isspace(ch);
            }));
            return ret;
        }

        // trim from end (in place)
        template<class T>
        std::basic_string<T> rtrim(std::basic_string<T> const& s)
        {
            std::basic_string<T> ret = s;
            ret.erase(std::find_if(ret.rbegin(), ret.rend(), [](int ch) {
                return !isspace(ch);
            }).base(), ret.end());
            return ret;
        }

        // trim from both ends (in place)
        template<class T>
        std::basic_string<T> trim(std::basic_string<T> const &s)
        {
            return ltrim(rtrim(s));
        }

        template<class T>
        std::basic_string<T> lowercase(std::basic_string<T> const& s)
        {
            std::basic_string<T> result = s;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return result;
        }

        template<class T>
        std::basic_string<T> uppercase(std::basic_string<T> const& s)
        {
            std::basic_string<T> result = s;
            std::transform(result.begin(), result.end(), result.begin(), ::toupper);
            return result;
        }
    };
}
