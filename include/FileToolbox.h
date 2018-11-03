#pragma once

namespace Toolbox
{
    class FileToolbox
    {
    public:
        FileToolbox() = delete;
        ~FileToolbox() = delete;

        static std::wstring ToAbsolutePath( std::wstring const& path );

        static std::wstring SetExtension( std::wstring const& file, std::wstring const& ext = L"txt" );

        static void Write( std::wstring const& file, std::vector<std::wstring> content, bool append = false );

        static std::vector<std::wstring> Read( std::wstring const & file );

        static bool IsRoot(std::wstring const& path );

        static std::wstring GetCurrentPath();

        static std::wstring GetFileName(std::wstring const& path);

        static std::wstring GetFolder( std::wstring const& path );

        static bool Exists( std::wstring const& path );
    };
}

