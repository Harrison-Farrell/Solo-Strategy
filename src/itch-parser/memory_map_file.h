#ifndef SOLO_STRATEGY_SRC_ITCH_PARSER_MEMORY_MAP_FILE_H_
#define SOLO_STRATEGY_SRC_ITCH_PARSER_MEMORY_MAP_FILE_H_

#ifdef __unix__
    // Linux
    #include <errno.h>
    #include <fcntl.h>
    #include <stdint.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <unistd.h>

    // enable large file support on 32 bit systems
    #ifndef _LARGEFILE64_SOURCE
        #define _LARGEFILE64_SOURCE
    #endif
    #ifdef _FILE_OFFSET_BITS
        #undef _FILE_OFFSET_BITS
    #endif
    #define _FILE_OFFSET_BITS 64
#elif defined(_WIN32) || defined(WIN32)
    // Windows
    #define __WINDOWS_OS__
    #include <windows.h>
typedef unsigned __int64 uint64_t;
#endif

#include <string>

class MemoryMappedFile {
   public:
    /// tweak performance
    enum CacheHint {
        Normal,          ///< good overall performance
        SequentialScan,  ///< read file only once with few seeks
        RandomAccess     ///< jump around
    };
    /// do nothing, must use open()
    MemoryMappedFile();

    /// open file, mappedBytes = 0 maps the whole file
    MemoryMappedFile(const std::string& filename, size_t mappedBytes = WholeFile,
                     CacheHint hint = Normal);

    /// close file (see close() )
    ~MemoryMappedFile();

    /// how much should be mappend
    enum MapRange { WholeFile = 0 };

    /// open file, mappedBytes = 0 maps the whole file
    bool open(const std::string& filename, size_t mappedBytes = WholeFile, CacheHint hint = Normal);

    /// close file
    void close();

    /// access position, no range checking (faster)
    unsigned char operator[](size_t offset) const;
    /// access position, including range checking
    unsigned char at(size_t offset) const;

    /// raw access
    const unsigned char* getData() const;

    // Iterator interface
    /// Begin interface pointer
    const char* begin() const;

    /// End interface pointer
    const char* end() const;

    /// true, if file successfully opened
    bool isValid() const;

    /// get file size
    uint64_t size() const;

    /// get number of actually mapped bytes
    size_t mappedSize() const;

    /// replace mapping by a new one of the same file, offset MUST be a multiple of the page size
    bool remap(uint64_t offset, size_t mappedBytes);

   private:
    /// don't copy object
    MemoryMappedFile(const MemoryMappedFile&);
    /// don't copy object
    MemoryMappedFile& operator=(const MemoryMappedFile&);

    /// get OS page size (for remap)
    static int getpagesize();

    /// file name
    std::string _filename;
    /// file size
    uint64_t _filesize;
    /// caching strategy
    CacheHint _hint;
    /// mapped size
    size_t _mappedBytes;

/// define handle
#ifdef __WINDOWS_OS__
    typedef void* FileHandle;
    /// Windows handle to memory mapping of _file
    void* _mappedFile;
#else
    typedef int FileHandle;
#endif
    /// file handle
    FileHandle _file;
    /// pointer to the file contents mapped into memory
    void* _mappedView;
};

#endif  // SOLO_STRATEGY_SRC_ITCH_PARSER_MEMORY_MAP_FILE_H_