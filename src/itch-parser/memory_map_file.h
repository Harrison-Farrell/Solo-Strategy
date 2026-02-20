/*
 * --------------------------------------------------------------------------
 * Author:      Harrison Farrell
 * Project:     Solo-Strategy Trading System
 * Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
 *
 * Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
 * This program is distributed WITHOUT ANY WARRANTY; without even the 
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
 * --------------------------------------------------------------------------
 */

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
#endif

#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <string>

#include "itch-parser/endian_utils.h"

class MemoryMappedFile {
   public:
    /// tweak performance
    enum CacheHint {
        Normal,
        SequentialScan,
        RandomAccess
    };

    /**
     * @brief Default constructor. Must use open() to map a file.
     */
    MemoryMappedFile();

    /**
     * @brief Opens a file and maps its contents into memory.
     * @param filename Path to the file.
     * @param mappedBytes Number of bytes to map. 0 maps the entire file.
     * @param hint Cache hint for performance tuning.
     */
    MemoryMappedFile(const std::filesystem::path& filename, size_t mappedBytes = WholeFile,
                     CacheHint hint = Normal);

    /**
     * @brief Closes the file and unmaps the memory (destructor).
     */
    ~MemoryMappedFile();

    /// how much should be mappend
    enum MapRange { WholeFile = 0 };

    /**
     * @brief Opens a file and maps its contents into memory.
     * @param filename Path to the file.
     * @param mappedBytes Number of bytes to map. 0 maps the entire file.
     * @param hint Cache hint for performance tuning.
     * @return true if successful, false otherwise.
     */
    [[nodiscard]] bool open(const std::filesystem::path& filename, size_t mappedBytes = WholeFile,
                            CacheHint hint = Normal);

    /**
     * @brief Closes the file and unmaps the memory.
     */
    void close();

    /**
     * @brief Accesses the data at the specified offset without range checking.
     * @param offset Offset from the beginning of the file.
     * @return The byte at the specified offset.
     */
    [[nodiscard]] uint8_t operator[](size_t offset) const;

    /**
     * @brief Accesses the data at the specified offset with range checking.
     * @param offset Offset from the beginning of the file.
     * @return The byte at the specified offset.
     * @throws std::out_of_range if the offset is beyond the file size.
     */
    [[nodiscard]] uint8_t at(size_t offset) const;

    /**
     * @brief Provides raw access to the mapped data.
     * @return A pointer to the beginning of the mapped data.
     */
    [[nodiscard]] const uint8_t* getData() const;

    /**
     * @brief Iterator to the beginning of the file content.
     */
    [[nodiscard]] const uint8_t* begin() const;

    /**
     * @brief Iterator to the end of the file content.
     */
    [[nodiscard]] const uint8_t* end() const;

    /**
     * @brief Checks if a file is successfully opened and mapped.
     */
    [[nodiscard]] bool isValid() const;

    /**
     * @brief Returns the total size of the file.
     */
    [[nodiscard]] uint64_t size() const;

    /**
     * @brief Returns the number of bytes currently mapped into memory.
     */
    [[nodiscard]] size_t mappedSize() const;

    /**
     * @brief Replaces the current mapping with a new one from the same file.
     * @param offset Start offset in the file. Must be a multiple of the page size.
     * @param mappedBytes Number of bytes to map.
     * @return true if successful, false otherwise.
     */
    [[nodiscard]] bool remap(uint64_t offset, size_t mappedBytes);

    // Generic read function for 8, 16, 32, or 64-bit values
    /**
     * @brief Reads a value of type T from the current cursor position.
     * @return The value read.
     * @throws std::out_of_range if reading past the end of the file.
     */
    template <typename T>
    T read() {
        if (mCursor + sizeof(T) > mFilesize) {
            throw std::out_of_range("Attempted to read past end of file.");
        }

        // Direct memory access via pointer casting
        T value = *reinterpret_cast<const T*>(static_cast<const uint8_t*>(mMappedView) + mCursor);
        mCursor += sizeof(T);
        return value;
    }

    /**
     * @brief Reads a Big-Endian value and converts it to host endianness.
     */
    template <typename T>
    T readBE() {
        return endian::from_big_endian(read<T>());
    }

    /**
     * @brief Reads a Little-Endian value and converts it to host endianness.
     */
    template <typename T>
    T readLE() {
        return endian::from_little_endian(read<T>());
    }

    // Specific helpers for your request
    // Specific helpers for ITCH (Big-Endian)
    uint8_t read8() { return read<uint8_t>(); }
    uint16_t read16() { return readBE<uint16_t>(); }
    uint32_t read32() { return readBE<uint32_t>(); }
    uint64_t read64() { return readBE<uint64_t>(); }

    /**
     * @brief Seeks to a specific position in the file.
     */
    void seek(size_t pos) { mCursor = pos; }

    /**
     * @brief Returns the current cursor position.
     */
    [[nodiscard]] size_t tell() const { return mCursor; }

   private:
    /** @brief Prevents copying. */
    MemoryMappedFile(const MemoryMappedFile&) = delete;
    /** @brief Prevents assignment. */
    MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;

    /** @brief Internal OS-specific helpers. */
    [[nodiscard]] bool osOpen(const std::filesystem::path& filename);
    void osClose();
    [[nodiscard]] bool osMap(uint64_t offset, size_t mappedBytes);
    void osUnmap();
    [[nodiscard]] uint64_t osGetFileSize() const;

    /** @brief Returns OS page size. */
    [[nodiscard]] static uint32_t getPageSize();

    /** @brief Current file name. */
    std::filesystem::path mFilename;
    /** @brief Total file size in bytes. */
    uint64_t mFilesize = 0;
    /** @brief Current read cursor position. */
    size_t mCursor = 0;
    /** @brief Caching hint. */
    CacheHint mHint = Normal;
    /** @brief Number of currently mapped bytes. */
    size_t mMappedBytes = 0;

#ifdef __WINDOWS_OS__
    using FileHandle = void*;
    /** @brief Windows handle to the underlying file. */
    FileHandle mFileHandle = nullptr;
    /** @brief Windows handle to the memory mapping object. */
    FileHandle mMappingHandle = nullptr;
#else
    using FileHandle = int;
    /** @brief Unix file descriptor. */
    FileHandle mFileHandle = -1;
#endif

    /** @brief Pointer to the mapped content. */
    void* mMappedView = nullptr;
};

#endif  // SOLO_STRATEGY_SRC_ITCH_PARSER_MEMORY_MAP_FILE_H_