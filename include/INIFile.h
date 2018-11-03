#pragma once
#include "File.h"

namespace File
{
    class CINIFile : public File
    {
    public:
        /*
        * INI Constructor
        *
        * @param : path and name of the file you want to use
        */
        explicit CINIFile( std::wstring const& path );

        /*
         * Append an ini file section at the end of the file.
         * Format is "[section]"
         * If the section already exist, an error will be thrown
         * 
         * @param : name of the section
         */
        void AppendSection( std::wstring const& name ) const;

        /*
        * Append an ini file comment at the end of the file.
        * Format is "; comment..."
        *
        * @param : comment text
        */
        void AppendComment( std::wstring const& comment ) const;

        /*
        * Append an empty line at the end of the file.
        */
        void AppendEmptyLine() const;

        /*
        * Append an ini file key and value at the end of the file.
        * Format is "key=value"
        * If the key already exist, an error will be thrown
        *
        * @param : key string used
        * @param : value linked to the key
        */
        void AppendKeyValue( std::wstring const& key, std::wstring const& value ) const;
        void AppendKeyValue( std::wstring const& key, int value ) const;
        void AppendKeyValue( std::wstring const& key, double value ) const;

        /*
        * Retrieve a value from it's key
        * If the key do not exist, return an empty value
        *
        * @param : key string used
        * @return : value linked to the key
        */
        std::wstring operator[]( std::wstring const& key ) const;

        /*
        * Set an existing value with the corresponding key
        * If the key do not exist, it will append the key=value at the end of the file
        *
        * @param : key string used
        * @return : value linked to the key
        */
        void SetValue( std::wstring const& key, std::wstring const& value ) const;
        void SetValue( std::wstring const& key, int value ) const;
        void SetValue( std::wstring const& key, double value ) const;

        /*
        * Retrieve a map of all keys and values from a section by name
        * If the section do not exists, will return an empty map
        *
        * @param : name of the section
        * @return : map of <key, value> in string format
        */
        std::map<std::wstring, std::wstring> GetSection( std::wstring const& name ) const;

    private:
        /*
        * Cache value for the last value returned by []
        */
        mutable std::wstring LastValue;

        /*
        * Cache index for the last key returned by []
        */
        mutable std::wstring LastKey;
    };
}
