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

        std::vector<std::wstring> Split( std::wstring const& string, std::wstring const & delimiter );
        std::vector<std::string> Split( std::string const & string, std::string const & delimiter );
        std::vector<std::wstring> Split( std::wstring const& string, wchar_t const & delimiter );
        std::vector<std::string> Split( std::string const& string, char const & delimiter );
        std::wstring replace(std::wstring const& str, const std::wstring& from, const std::wstring& to);

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
        static inline void ltrim(std::string &s)
        {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                return !isspace(ch);
            }));
        }

        // trim from end (in place)
        static inline void rtrim(std::string &s)
        {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
                return !isspace(ch);
            }).base(), s.end());
        }

        // trim from both ends (in place)
        static inline std::string trim(std::string const &s)
        {
            std::string ret = s;
            ltrim(ret);
            rtrim(ret);
            return ret;
        }

        static inline std::string lowercase(std::string const& s)
        {
            std::string result = s;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return result;
        }

        static inline std::string uppercase(std::string const& s)
        {
            std::string result = s;
            std::transform(result.begin(), result.end(), result.begin(), ::toupper);
            return result;
        }
    };
}
