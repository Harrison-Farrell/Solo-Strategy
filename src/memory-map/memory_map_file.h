// -----------------------------------------------------------------------------
// Author:      Harrison Farrell
// Project:     Solo-Strategy Trading System
// Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
//
// Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
// This program is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
// -----------------------------------------------------------------------------

#ifndef SOLO_STRATEGY_SRC_MEMORY_MAP_FILE_H_
#define SOLO_STRATEGY_SRC_MEMORY_MAP_FILE_H_

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
#endif

#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include <string>

#include "utilities/endian_utils.h"

/// @class MemoryMappedFile
/// @brief Provides a high-performance, read-only memory mapping of a file.
///
/// Memory-mapped files allow applications to access file content as if it were
/// in-memory arrays. This is often faster than traditional I/O for large files
/// and large-scale data processing like ITCH parsing.
///
/// The class supports both Windows and Unix-like operating systems and provides
/// safe cursor-based reading with automatic endian conversion.
class MemoryMappedFile {
   public:
    /// @brief Performance hints for the OS memory manager.
    enum CacheHint {
        Normal,          ///< Default OS behavior.
        SequentialScan,  ///< Optimizes for sequential reading.
        RandomAccess     ///< Disables aggressive prefetching.
    };

    /// @brief Default constructor. File must be opened using open().
    MemoryMappedFile();

    /// @brief Prevents creating duplicates of the mapping object.
    MemoryMappedFile(const MemoryMappedFile&) = delete;
    /// @brief Prevents assignment, as handles are managed uniquely.
    MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;

    /// @brief Constructs and immediately maps a file.
    /// @param filename Path to the file.
    /// @param mappedBytes Number of bytes to map. Use WholeFile (0) for the
    /// entire file.
    /// @param hint Performance hint for the operating system.
    /// @throws std::runtime_error If the file fails to open or map.
    MemoryMappedFile(const std::filesystem::path& filename,
                     size_t mappedBytes = WholeFile, CacheHint hint = Normal);

    /// @brief Unmaps the memory and closes the file handles.
    ~MemoryMappedFile();

    /// @brief Value used to represent the entire file length in mapping calls.
    enum MapRange { WholeFile = 0 };

    /// @brief Opens a file and maps it into memory.
    /// @param filename Path to the file.
    /// @param mappedBytes Bytes to map. 0 means map everything.
    /// @param hint Performance hint for the OS.
    /// @return true if successful.
    [[nodiscard]] bool open(const std::filesystem::path& filename,
                            size_t mappedBytes = WholeFile,
                            CacheHint hint = Normal);

    /// @brief Unmaps the memory and closes all handles. Safe to call multiple
    /// times.
    void close();

    /// @brief Fast, unchecked array-style access to the mapped data.
    /// @param offset Byte offset from the start of the mapping.
    /// @return Byte value at the specified offset.
    /// @warning No bounds checking for performance. Ensure offset < size().
    [[nodiscard]] uint8_t operator[](size_t offset) const;

    /// @brief Checked access to the mapped data.
    /// @param offset Byte offset from the start of the mapping.
    /// @return Byte value at the specified offset.
    /// @throws std::out_of_range If offset is >= size().
    [[nodiscard]] uint8_t at(size_t offset) const;

    /// @brief Returns a raw pointer to the start of the memory mapping.
    /// @return Continuous memory pointer to the file content.
    [[nodiscard]] const uint8_t* getData() const;

    /// @brief STL-compatible iterator to the start of the mapped data.
    /// @return Constant pointer to the beginning.
    [[nodiscard]] const uint8_t* begin() const;

    /// @brief STL-compatible iterator to the byte after the last mapped byte.
    /// @return Constant pointer to the end.
    [[nodiscard]] const uint8_t* end() const;

    /// @brief Checks if the file is currently validly mapped.
    /// @return true if mapping is active.
    [[nodiscard]] bool isValid() const;

    /// @brief Gets the total size of the underlying file on disk.
    /// @return File size in bytes.
    [[nodiscard]] uint64_t size() const;

    /// @brief Gets the number of bytes currently mapped into the process's
    /// address space.
    /// @return Number of mapped bytes.
    [[nodiscard]] size_t mappedSize() const;

    /// @brief Shifts the memory mapping window.
    /// @param offset New start offset in the file. Must be a multiple of OS
    /// page size.
    /// @param mappedBytes Number of bytes to map from the new offset.
    /// @return true on success.
    [[nodiscard]] bool remap(uint64_t offset, size_t mappedBytes);

    /// @brief Inpsects a value of type T from the current cursor.
    /// @tparam T Type to read (e.g., uint32_t, char).
    /// @return Value read from the stream.
    /// @throws std::out_of_range If reading past end of mapped region.
    template <typename T>
    T Inspect() {
        // Direct memory access via pointer casting
        T value = *reinterpret_cast<const T*>(
            static_cast<const uint8_t*>(mMappedView) + mCursor);
        return value;
    }

    /// @brief Reads a value of type T from the current cursor and advances it.
    /// @tparam T Type to read (e.g., uint32_t, char).
    /// @return Value read from the stream.
    /// @throws std::out_of_range If reading past end of mapped region.
    template <typename T>
    T Read() {
        // Direct memory access via pointer casting
        T value = *reinterpret_cast<const T*>(
            static_cast<const uint8_t*>(mMappedView) + mCursor);
        mCursor += sizeof(T);
        return value;
    }

    /// @brief Reads Big-Endian data and converts to host endianness.
    /// @tparam T Integer type to read.
    /// @return Host-endian value.
    template <typename T>
    T ReadBE() {
        return endian::FromBigEndian(Read<T>());
    }

    /// @brief Reads Little-Endian data and converts to host endianness.
    /// @tparam T Integer type to read.
    /// @return Host-endian value.
    template <typename T>
    T ReadLE() {
        return endian::FromLittleEndian(Read<T>());
    }

    /// @brief Reads a single byte.
    /// @return uint8_t value.
    /// @warning No bounds checking for performance. Ensure offset < size().
    uint8_t Read8() { return Read<uint8_t>(); }
    /// @brief Reads a single byte.
    /// @return char value.
    /// @warning No bounds checking for performance. Ensure offset < size().
    char ReadChar() { return Read<char>(); }
    /// @brief Reads a 16-bit big-endian value.
    /// @return uint16_t value.
    /// @warning No bounds checking for performance. Ensure offset < size().
    uint16_t Read16() { return ReadBE<uint16_t>(); }
    /// @brief Reads a 32-bit big-endian value.
    /// @return uint32_t value.
    /// @warning No bounds checking for performance. Ensure offset < size().
    uint32_t Read32() { return ReadBE<uint32_t>(); }
    /// @brief Reads a 48-bit big-endian value.
    /// @return uint64_t value.
    /// @warning No bounds checking for performance. Ensure offset < size().
    uint64_t Read48();
    /// @brief Reads a 64-bit big-endian value.
    /// @return uint64_t value.
    /// @warning No bounds checking for performance. Ensure offset < size().
    uint64_t Read64() { return ReadBE<uint64_t>(); }

    template <size_t N>
    std::array<char, N> ReadArray() {
        std::array<char, N> result{};
        std::memcpy(result.data(),
                    static_cast<const char*>(mMappedView) + mCursor, N);
        mCursor += N;
        return result;
    }

    /// @brief Copies a fixed-length string and advances the cursor.
    /// @param dentation of the string copy.
    /// @param length Number of characters to read.
    inline auto copyString(char* destination, size_t length) -> void {
        std::memcpy(destination,
                    static_cast<const char*>(mMappedView) + mCursor, length);
        mCursor += length;
    }

    /// @brief Memory-mapped reading of an 8-character symbol.
    /// @return std::string containing 8 characters.
    std::array<char, 8> ReadSymbol() { return ReadArray<8>(); }
    /// @brief Sets the internal cursor for the read() functions.
    /// @param pos New byte offset from the start of the file.
    void seek(size_t pos) { mCursor = pos; }
    /// @brief Gets current byte position of the cursor.
    /// @return Cursor offset in bytes.
    [[nodiscard]] size_t tell() const { return mCursor; }

   private:
    /// @brief Implementation detail for OS-specific file opening.
    [[nodiscard]] bool osOpen(const std::filesystem::path& filename);
    /// @brief Implementation detail for OS-specific handle closing.
    void osClose();
    /// @brief Implementation detail for OS-specific mapping.
    [[nodiscard]] bool osMap(uint64_t offset, size_t mappedBytes);
    /// @brief Implementation detail for OS-specific unmapping.
    void osUnmap();
    /// @brief Implementation detail for OS-specific file size retrieval.
    [[nodiscard]] uint64_t osGetFileSize() const;

    /// @brief Helper to retrieve the granularity required for mmap offsets.
    [[nodiscard]] static uint32_t getPageSize();

    std::filesystem::path mFilename;  ///< Canonical path of the opened file.
    uint64_t mFilesize = 0;           ///< Cached size of the file in bytes.
    size_t mCursor = 0;  ///< Current stream position for sequential reads.
    CacheHint mHint = Normal;  ///< Active performance hint.
    size_t mMappedBytes = 0;   ///< Actual bytes currently accessible in memory.

#ifdef __WINDOWS_OS__
    using FileHandle = void*;
    FileHandle mFileHandle = nullptr;  ///< Native Windows file handle.
    FileHandle mMappingHandle =
        nullptr;  ///< Native Windows mapping object handle.
#else
    using FileHandle = int;
    FileHandle mFileHandle = -1;  ///< POSIX file descriptor.
#endif

    void* mMappedView = nullptr;  ///< Pointer to shared memory region.
};

#endif  // SOLO_STRATEGY_SRC_MEMORY_MAP_FILE_H_