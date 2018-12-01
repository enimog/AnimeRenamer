#pragma once

#include <filesystem>

namespace Toolbox
{
    namespace FileToolbox
    {
        std::wstring ToAbsolutePath( std::wstring const& path );
        std::string  ToAbsolutePath( std::string  const& path );

        std::wstring GetExtension( std::wstring const& file );

        std::wstring SetExtension( std::wstring const& file, std::wstring const& ext = L"txt" );

        void Write( std::wstring const& file, std::vector<std::wstring> content, bool append = false );

        std::vector<std::wstring> Read( std::wstring const & file );

        bool IsRoot(std::wstring const& path );

        std::wstring GetCurrentPath();

        std::wstring GetFileName(std::wstring const& path);

        std::wstring GetFolder( std::wstring const& path );

        bool Exists( std::wstring const& path );

        template<class T1, class T2>
        bool Move(std::basic_string<T1> const& from, std::basic_string<T2> const& to)
        {
            using namespace std::filesystem;
            try
            {
                // Will try to rename if possible
                try
                {
                    create_directories(path(to).parent_path());
                    rename(from, to);
                }
                // In some cases (eg. transfer between drives) it is not possible to rename.
                // Instead, will try to copy and then delete the file.
                catch(filesystem_error const&)
                {
                    create_directories(path(to).parent_path());
                    copy(from, to);
                    remove(from);
                }
                return true;
            }
            catch(filesystem_error const&)
            {
                return false;
            }
        }
    };
}

