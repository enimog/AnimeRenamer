#pragma once
#include <stddef.h>
#include "FileWatcherThread.h"
#include "FileWriterThread.h"
#include <mutex>

namespace File
{
    class File : public CObservable
    {
    public:
        /*
         * File class constructor
         * This class represent a "pointer" to a physical file on the disk. This means that
         * all instances of this class using the same path will share the same data since they all
         * reference the same physical file.
         */
        explicit File(std::wstring const& path);
        File( const File &obj );

        File& operator=( const File& other );
        bool operator==( const File& other ) const;
        bool operator==( const std::wstring& path ) const;

        /*
         * Common operation for all file types ; it empties the physical file content
         */
        void clear() const;

        /*
         * Copy the file content from the argument to the physical file this object point to.
         *
         * @param : file that you want to copy it's data into this file
         */
        void copy_from( const File& file ) const;

        ~File();

    protected:

        /*
         * get the logical data for a file (not the one on the hard drive).
         * This function is used to give access to the file data to all
         * child classes to interpret it.
         *
         * @return : vector corresponding to all lines inside the file
         */
        const std::vector<std::wstring>& getData() const;

        /*
        * set the logical data for a file (not the one on the hard drive).
        * This function is used to give access to the file data to all
        * child classes to modify it.
        *
        * @param : vector corresponding to all lines inside the file
        */
        void setData( const std::vector<std::wstring>& data) const;

    private:

        /*
         * Structure containing objects that are specific for a physical file
         * instead of the class.
         */
        struct per_file_members
        {
            size_t RefCount = 0;
            std::mutex Mutex;
            std::unique_ptr<CFileWatcherThread> WatchThread;
            std::unique_ptr<CFileWriterThread> WriteThread;
            std::vector<std::wstring> LocalContent;
        };

        /*
         * Local variable containing the path of the file this class is referencing
         */
        std::wstring m_strPath;

        /*
         * Static map containing all physical files currently used and their properties.
         * It is used internally in that all operations done by this class happens on the
         * physical representation of the file.
         *
         * @key : file path
         * @value : the data structure shared among all files references to a unique file
         */
        static std::map<std::wstring, per_file_members> m_FileMap;

        /*
         * Callback when a file is changed. Will notify all observers
         */
        void OnChange() const;

        /*
         * Function used to lower the reference count and unregister all callback if necessary.
         */
        void removeReference();
    };
}
