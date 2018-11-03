#pragma once

namespace Toolbox
{
    namespace FileToolbox
    {
        std::wstring ToAbsolutePath( std::wstring const& path );

        std::wstring GetExtension( std::wstring const& file );

        std::wstring SetExtension( std::wstring const& file, std::wstring const& ext = L"txt" );

        void Write( std::wstring const& file, std::vector<std::wstring> content, bool append = false );

        std::vector<std::wstring> Read( std::wstring const & file );

        bool IsRoot(std::wstring const& path );

        std::wstring GetCurrentPath();

        std::wstring GetFileName(std::wstring const& path);

        std::wstring GetFolder( std::wstring const& path );

        bool Exists( std::wstring const& path );
    };
}

