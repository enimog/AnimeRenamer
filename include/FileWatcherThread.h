#pragma once
#include <thread>
#include <mutex>
#include "CObservable.h"

class CFileWatcherThread : public CObservable
{
public:
    /*
     * File watcher thread
     * This class will create a thread that will listen to changes happening to the file.
     * When a change is detected, it will update the fileData with the new values and notify observers.
     * The mutex is here to protect the fileData buffer when updating the data.
     *
     * @param : path to the file being watched
     * @param : buffer to the data from the file
     * @param : mutex used to write-protect the file data
     */
    CFileWatcherThread(const std::wstring& path, std::vector<std::wstring>& fileData, std::mutex& mutex );
    ~CFileWatcherThread();

private:
    /*
    * Main loop used by the internal thread that is watching the file
    *
    * @param : path to the file being watched
    * @param : buffer to the data from the file
    * @param : mutex used to write-protect the file data
    */
    void FileWatchMainLoop( const std::wstring& path, std::vector<std::wstring>& fileData, std::mutex& mutex ) const;

#if defined(_WIN32) || defined(WIN32)
    /*
     * HANDLE equal to the directory being watched. Used by the win32 API call used internally
     */
    HANDLE m_Handle;
#endif

    /*
    * boolean value indicating that the thread is running
    * This value is used internally to forcefully stop the thread
    */
    bool m_bIsRunning;

    /*
    * Variable holding the internal worker thread for listening to file changes
    */
    std::unique_ptr<std::thread> m_Thread;
};
