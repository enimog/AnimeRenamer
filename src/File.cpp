#include "stdafx.h"
#include "File.h"
#include "FileToolbox.h"
#include <map>

namespace File
{
    std::map<std::wstring, File::per_file_members> File::m_FileMap;

    File::File( std::wstring const& path ) : m_strPath( Toolbox::FileToolbox::ToAbsolutePath( path ) )
    {
        m_FileMap[m_strPath].LocalContent = Toolbox::FileToolbox::Read( m_strPath );

        if (m_strPath.size() > 0)
        {
            if (m_FileMap[m_strPath].RefCount == 0)
            {
                m_FileMap[m_strPath].WatchThread = std::make_unique<CFileWatcherThread>( m_strPath, m_FileMap[m_strPath].LocalContent, m_FileMap[m_strPath].Mutex );
                m_FileMap[m_strPath].WriteThread = std::make_unique<CFileWriterThread>( m_strPath, m_FileMap[m_strPath].LocalContent, m_FileMap[m_strPath].Mutex );
            }
            m_FileMap[m_strPath].WatchThread->registerCallback( std::bind( &File::OnChange, this ) );

            m_FileMap[m_strPath].RefCount++;
        }

        LOG_TRACE( L"Constructor for file %s", m_strPath.c_str() );
    }

    File::File( const File & obj ) : File( obj.m_strPath )
    {
    }

    File & File::operator=( const File & other )
    {
        File::~File();

        File( other.m_strPath );

        return *this;
    }

    bool File::operator==( const File & other ) const
    {
        return *this == other.m_strPath;
    }

    bool File::operator==( const std::wstring & path ) const
    {
        return m_strPath.compare( path ) == 0;
    }

    void File::clear() const
    {
        setData( std::vector<std::wstring>() );
    }

    void File::copy_from( const File & file ) const
    {
        setData( file.getData() );
    }

    File::~File()
    {
        if (m_strPath.size() > 0)
        {
            m_FileMap[m_strPath].RefCount--;

            m_FileMap[m_strPath].WatchThread->unregisterCallback( std::bind( &File::OnChange, this ) );

            if (m_FileMap[m_strPath].RefCount == 0)
            {
                // I have to explicitely kill it before the rest because otherwise we won't be able to write
                // properly the remaining data
                m_FileMap[m_strPath].WriteThread.reset();

                m_FileMap.erase( m_FileMap.find( m_strPath ) );
            }
        }

        LOG_TRACE(L"Destructor for file %s", m_strPath.c_str() );
    }

    const std::vector<std::wstring>& File::getData() const
    {
        return m_FileMap[m_strPath].LocalContent;
    }

    void File::setData( const std::vector<std::wstring>& data ) const
    {
        if (m_strPath.size() > 0)
        {
            std::lock_guard<std::mutex> guard(m_FileMap[m_strPath].Mutex);
            m_FileMap[m_strPath].LocalContent = data;

            m_FileMap[m_strPath].WriteThread->notify();
        }
    }
    void File::OnChange() const
    {
        notify();
    }
}
