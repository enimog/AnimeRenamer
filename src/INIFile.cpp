#include "stdafx.h"
#include "INIFile.h"
#include "FileToolbox.h"

namespace File
{
    CINIFile::CINIFile( std::wstring const & path ) : File( Toolbox::FileToolbox::SetExtension( path, L"ini" ) )
    {
    }

    void CINIFile::AppendSection( std::wstring const & name ) const
    {
        auto dataToWrite = getData();
        auto lineToAdd = L"[" + name + L"]";

        for (auto const& line : dataToWrite)
        {
            if (line.compare( lineToAdd ) == 0)
            {
                throw Toolbox::StringToolbox::Format(L"Section already exist : %d", name.c_str());
            }
        }

        dataToWrite.push_back( lineToAdd );

        setData( dataToWrite );
    }

    void CINIFile::AppendComment( std::wstring const & comment ) const
    {
        auto dataToWrite = getData();

        dataToWrite.push_back( L"; " + comment );

        setData( dataToWrite );
    }

    void CINIFile::AppendEmptyLine() const
    {
        auto dataToWrite = getData();

        dataToWrite.push_back( std::wstring() );

        setData( dataToWrite );
    }

    void CINIFile::AppendKeyValue( std::wstring const & key, std::wstring const & value ) const
    {
        auto dataToWrite = getData();

        if ((*this)[key].size() > 0)
        {
            throw Toolbox::StringToolbox::Format( L"Key already exist : %d", key.c_str() );
        }

        dataToWrite.push_back( key + L"=" + value );

        setData( dataToWrite );
    }

    void CINIFile::AppendKeyValue( std::wstring const & key, int value ) const
    {
        AppendKeyValue( key, std::to_wstring( value ) );
    }

    void CINIFile::AppendKeyValue( std::wstring const & key, double value ) const
    {
        AppendKeyValue( key, std::to_wstring( value ) );
    }

    std::wstring CINIFile::operator[]( std::wstring const & key ) const
    {
        if (key.empty())
        {
            return std::wstring();
        }

        if (key.compare(LastKey) == 0)
        {
            return LastValue;
        }

        std::wstring result;
        auto dataToRead = getData();

        for (auto const& line : dataToRead)
        {
            auto pos = line.find(L"=");
            if (pos != std::wstring::npos && line.substr(0, pos).compare(key) == 0)
            {
                result = line.substr( pos + 1 );
                break;
            }
        }

        LastKey = key;
        LastValue = result;

        return result;
    }

    void CINIFile::SetValue( std::wstring const & key, std::wstring const & value ) const
    {
        auto dataToWrite = getData();

        for (auto& line : dataToWrite)
        {
            auto pos = line.find( L"=" );
            if (pos != std::wstring::npos && line.substr(0, pos).compare(key) == 0)
            {
                line = key + L"=" + value;

                setData( dataToWrite );

                return;
            }
        }

        AppendKeyValue( key, value );
    }

    void CINIFile::SetValue( std::wstring const & key, int value ) const
    {
        SetValue( key, std::to_wstring( value ) );
    }

    void CINIFile::SetValue( std::wstring const & key, double value ) const
    {
        SetValue( key, std::to_wstring( value ) );
    }

    std::map<std::wstring, std::wstring> CINIFile::GetSection( std::wstring const & name ) const
    {
        std::map<std::wstring, std::wstring> result;
        auto dataToRead = getData();
        auto SectionToFind = L"[" + name + L"]";
        auto bSectionFound = false;

        for (auto const& line : dataToRead)
        {
            if (bSectionFound)
            {
                // If we are to a new section
                if (line[0] == L'[')
                {
                    break;
                }

                // If there is a key=value on that line
                auto pos = line.find( L"=" );
                if (pos != std::wstring::npos)
                {
                    result[line.substr( 0, pos )] = line.substr( pos + 1 );
                }
            }
            // If we find the correct section
            else if (line.compare( SectionToFind ) == 0)
            {
                bSectionFound = true;
            }
        }

        return result;
    }
}
