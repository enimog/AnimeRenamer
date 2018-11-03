#include "stdafx.h"
#include "FileWatcherThread.h"
#include "FileToolbox.h"


CFileWatcherThread::CFileWatcherThread( const std::wstring& path, std::vector<std::wstring>& fileData, std::mutex& mutex ) :
#if defined(_WIN32) || defined(WIN32)
m_Handle(nullptr),
#endif
m_bIsRunning(true)
{
#if defined(_WIN32) || defined(WIN32)
    m_Handle = CreateFileW(
        Toolbox::FileToolbox::GetFolder( path ).c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        nullptr );
#endif
    m_Thread = std::make_unique<std::thread>( &CFileWatcherThread::FileWatchMainLoop, this, cref( path ), ref( fileData ), std::ref(mutex) );
}


CFileWatcherThread::~CFileWatcherThread()
{
    m_bIsRunning = false;
#if defined(_WIN32) || defined(WIN32)
    CloseHandle( m_Handle );
#endif
    if (m_Thread->joinable())
    {
        m_Thread->join();
    }
}

void CFileWatcherThread::FileWatchMainLoop( const std::wstring& path, std::vector<std::wstring>& fileData, std::mutex& mutex ) const
{
    if (path.size() > 0)
    {
#if defined(_WIN32) || defined(WIN32)
        auto strDirectory = Toolbox::FileToolbox::GetFolder( path );
        std::vector<char> buffer( MAX_PATH );
        unsigned long lpBytesReturned = 0;

        while (m_bIsRunning)
        {
            if (ReadDirectoryChangesW( m_Handle, &buffer[0], static_cast<unsigned long>(buffer.size()), false, FILE_NOTIFY_CHANGE_LAST_WRITE, &lpBytesReturned, nullptr, nullptr ) != 0)
            {
                auto& fni = reinterpret_cast<FILE_NOTIFY_INFORMATION&>(*buffer.data());

                std::wstring filename( fni.FileName, fni.FileNameLength / sizeof( wchar_t ) );

                if (path.compare( strDirectory + L"\\" + filename ) == 0)
                {
                    const auto& dataRead = Toolbox::FileToolbox::Read( path );

                    std::lock_guard<std::mutex> guard(mutex);
                    fileData = dataRead;

                    notify();
                }

            }
        }
#endif
    }
}
