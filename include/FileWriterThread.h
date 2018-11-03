#pragma once
#include <thread>
#include <condition_variable>
#include <mutex>

class CFileWriterThread
{
public:
    /*
     * This class is a specialized thread used to write data to disk.
     *
     * @param path : path to use when writing to disk.
     * @param fileData : linked data array to use when writing to disk.
     * @param mutex : mutex handle to lock write access when writing to disk.
     */
    CFileWriterThread( std::wstring path, const std::vector<std::wstring>& fileData, std::mutex& mutex );
    ~CFileWriterThread();

    /*
     * Notify the thread that fileData has been modified.
     * When notified, the thread will initiate a timer before finally writing to disk
     * The timer will be reset each time notify() is called. When the timer expires,
     * the array become write protected during the time the thread is writing to disk
     * the fileData.
     */
    void notify();

private:
    /*
     * This function is the thread main loop used internally.
     *
     * @param path : path to use when writing to disk.
     * @param fileData : linked data array to use when writing to disk.
     * @param mutex : mutex handle to lock write access when writing to disk.
     */
    void ThreadMainLoop( std::wstring path,const std::vector<std::wstring>& fileData, std::mutex& mutex );

    /*
     * Conditional flag that is set when there is a file update via notify(). Internally,
     * it force the thread to sleep permenently until there is an update to do on the disk.
     * This flag is used as a "wake up" flag for the thread.
     */
    std::condition_variable m_FileUpdateFlag;

    /*
     * Conditional flag that is used to wait a fixed time when notify is called(). When it expires,
     * the data is written to disk.
     */
    std::condition_variable m_WriteWait;

    /*
     * This time is used internally to determine how much time the thread should
     * sleep before writing to disk the data.
     */
    std::chrono::system_clock::time_point m_Timepoint;

    /*
     * boolean value indicating that the thread is running
     * This value is used internally to forcefully stop the thread
     */
    bool m_bIsRunning;

    /*
     * Variable holding the internal worker thread for writing to file
     */
    std::unique_ptr<std::thread> m_Thread;
};

