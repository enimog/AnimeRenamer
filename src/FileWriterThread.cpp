#include "stdafx.h"
#include "FileWriterThread.h"
#include "FileToolbox.h"


CFileWriterThread::CFileWriterThread( std::wstring path, const std::vector<std::wstring>& fileData, std::mutex& mutex ) :
    m_FileUpdateFlag(),
    m_WriteWait(),
    m_Timepoint(),
    m_bIsRunning( true )
{
    m_Thread = std::make_unique<std::thread>( &CFileWriterThread::ThreadMainLoop, this, path, cref( fileData ), std::ref( mutex ) );
}


CFileWriterThread::~CFileWriterThread()
{
    m_bIsRunning = false;
    m_Timepoint = std::chrono::system_clock::now();
    m_FileUpdateFlag.notify_all();
    m_WriteWait.notify_all();

    if (m_Thread->joinable())
    {
        m_Thread->join();
    }
}

void CFileWriterThread::notify()
{
    m_Timepoint = std::chrono::system_clock::now() + std::chrono::seconds( 1 );
    m_FileUpdateFlag.notify_all();
}

void CFileWriterThread::ThreadMainLoop( std::wstring path, const std::vector<std::wstring>& fileData, std::mutex& file_mutex )
{
    std::mutex mtx;

    std::unique_lock<std::mutex> lck( mtx );

    while (m_bIsRunning)
    {
        m_FileUpdateFlag.wait( lck );
        m_WriteWait.wait_until( lck, m_Timepoint );

        std::lock_guard<std::mutex> guard(file_mutex);
        Toolbox::FileToolbox::Write( path, fileData );

        LOG_TRACE( L"Written to file %s", path.c_str() );
    }
}
