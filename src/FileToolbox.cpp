#include "stdafx.h"
#include "FileToolbox.h"
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <whereami/whereami.h>

std::wstring Toolbox::FileToolbox::ToAbsolutePath( std::wstring const & path )
{
    auto path_no_const( path );
    replace( path_no_const.begin(), path_no_const.end(), L'\\', L'/' );

    auto directories = StringToolbox::Split( path_no_const, L'/' );

    // If the path doesn't begin with a root directory, append the current path
    if (!IsRoot(directories[0] + L'/' ))
    {
        auto current_path = StringToolbox::Split( GetCurrentPath(), L'/' );
        current_path.insert( current_path.end(), directories.begin(), directories.end() );
        directories = current_path;
    }

    // replace .. and ., and rebuild the path
#if defined(_WIN32) || defined(WIN32)
    std::wstring result = L"";
#else
    std::wstring result = L"/";
#endif
    for (size_t index = 0; index < directories.size(); index++)
    {
        if (directories[index].length() > 0 && directories[index].compare(L".") != 0)
        {
            if (directories[index].compare( L".." ) == 0)
            {
                if (!IsRoot( directories[index] + L'/' ))
                {
                    auto test = result.substr( 0, result.find_last_of( L'/' ) );
                    auto test2 = test.substr( 0, test.find_last_of( L'/' ) );
                    result = test2 + L'/';
                }
            }
            else
            {
                result += directories[index];

                if (index + 1 < directories.size())
                {
                    result += L'/';
                }
            }
        }
    }

    return result;
}

std::wstring Toolbox::FileToolbox::SetExtension( std::wstring const & path, std::wstring const & ext )
{
    std::wstring result;

    if (path.find(L'.') != std::wstring::npos)
    {
        auto pos = path.find_last_of( L'.' );
        auto without_ext = path.substr( 0, pos );
        result = without_ext + L'.' + ext;
    }
    // There is no extension, so we simply append it
    else
    {
        result = path + L'.' + ext;
    }

    return result;
}

void Toolbox::FileToolbox::Write( std::wstring const & file, std::vector<std::wstring> content, bool append )
{
    FILE* file_handle = nullptr;

    if (append)
    {
        file_handle = fopen( StringToolbox::WideToString(file).c_str(), "a" );
    }
    else
    {
        file_handle = fopen( StringToolbox::WideToString(file).c_str(), "w" );
    }

    for (auto& line : content)
    {
        fputws( (line + L'\n').c_str(), file_handle );
    }

    fclose( file_handle );
}

std::vector<std::wstring> Toolbox::FileToolbox::Read( std::wstring const & file )
{
    std::vector<std::wstring> result;
    std::wifstream infile(StringToolbox::WideToString(file), std::ios::binary );

    std::wstring line = L"";
    for (wchar_t c; infile.get( c ); )
    {
        if (c == L'\n')
        {
            result.push_back( line );
            line = L"";
        }
        else if (c != L'\r')
        {
            line += c;
        }
    }

    if (line.size() > 0)
    {
        result.push_back( line );
    }

    return result;
}

#if defined(_WIN32) || defined(WIN32)
bool Toolbox::FileToolbox::IsRoot( std::wstring const & path )
{
    wchar_t szLogicalDrives[MAX_PATH];
    auto dwResult = GetLogicalDriveStrings( MAX_PATH, szLogicalDrives );

    if (dwResult > 0 && dwResult <= MAX_PATH)
    {
        auto* szSingleDrive = szLogicalDrives;
        while (*szSingleDrive)
        {
            if (path.compare( szSingleDrive ) == 0)
            {
                return true;
            }

            // get the next drive
            szSingleDrive += wcslen( szSingleDrive ) + 1;
        }
    }

    return false;
}
#else
bool Toolbox::FileToolbox::IsRoot( std::wstring const & path )
{
    return !path.empty() && path[0] == L'/';
}
#endif

std::wstring Toolbox::FileToolbox::GetCurrentPath()
{
    char path[MAX_PATH];
    const auto length = wai_getExecutablePath(path, MAX_PATH, nullptr);
    std::wstring full_path(&path[0], &path[length]);
    replace( full_path.begin(), full_path.end(), L'\\', L'/' );

    return full_path.substr(0, full_path.rfind(L'/'));
}

std::wstring Toolbox::FileToolbox::GetFileName( std::wstring const& path )
{
    auto result( path );
    replace( result.begin(), result.end(), L'\\', L'/' );

    return StringToolbox::Split( result, L'/' ).back();
}

std::wstring Toolbox::FileToolbox::GetFolder( std::wstring const & path )
{
    const auto tmp( ToAbsolutePath( path ) );

    return tmp.substr(0, tmp.find_last_of(L"\\/" ));
}

bool Toolbox::FileToolbox::Exists( std::wstring const & path )
{
    return std::filesystem::exists( ToAbsolutePath(path) );
}
