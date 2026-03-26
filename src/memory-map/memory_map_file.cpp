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

#include "memory-map/memory_map_file.h"

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <stdexcept>

#ifdef __WINDOWS_OS__
    #define WIN32_LEAN_AND_MEAN
    #include <fileapi.h>
    #include <handleapi.h>
    #include <memoryapi.h>
    #include <minwindef.h>
    #include <sysinfoapi.h>
    #include <winnt.h>
#else
    #include <errno.h>
    #include <fcntl.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif

MemoryMappedFile::MemoryMappedFile() = default;

MemoryMappedFile::MemoryMappedFile(const std::filesystem::path& filename,
                                   size_t mapped_bytes, CacheHint hint) {
    if (!Open(filename, mapped_bytes, hint)) {
        throw std::runtime_error(
            "MemoryMappedFile::MemoryMappedFile() : failed to open file");
    }
}

MemoryMappedFile::~MemoryMappedFile() { Close(); }

bool MemoryMappedFile::Open(const std::filesystem::path& filename,
                            size_t mapped_bytes, CacheHint hint) {
    Close();

    if (!osOpen(filename)) {
        return false;
    }

    mFilename = filename;
    mHint = hint;
    mFilesize = osGetFileSize();

    if (mFilesize == 0) {
        osClose();
        return false;
    }

    if (!osMap(0, mapped_bytes)) {
        osClose();
        return false;
    }

    return true;
}

void MemoryMappedFile::Close() {
    osUnmap();
    osClose();

    mFilename.clear();
    mFilesize = 0;
    mMappedBytes = 0;
    mCursor = 0;
}

bool MemoryMappedFile::remap(uint64_t offset, size_t mapped_bytes) {
    if (!isValid()) {
        return false;
    }

    if (offset + mapped_bytes > mFilesize &&
        mapped_bytes != static_cast<size_t>(MapRange::WholeFile)) {
        return false;
    }

    osUnmap();
    return osMap(offset, mapped_bytes);
}

uint8_t MemoryMappedFile::operator[](size_t offset) const {
    return static_cast<const uint8_t*>(mMappedView)[offset];
}

uint8_t MemoryMappedFile::at(size_t offset) const {
    if (offset >= mMappedBytes) {
        throw std::out_of_range("MemoryMappedFile::at() : offset out of range");
    }
    return operator[](offset);
}

const uint8_t* MemoryMappedFile::getData() const {
    return static_cast<const uint8_t*>(mMappedView);
}

const uint8_t* MemoryMappedFile::begin() const { return getData(); }

const uint8_t* MemoryMappedFile::end() const {
    return getData() + mMappedBytes;
}

bool MemoryMappedFile::isValid() const { return mMappedView != nullptr; }

uint64_t MemoryMappedFile::size() const { return mFilesize; }

size_t MemoryMappedFile::mappedSize() const { return mMappedBytes; }

uint64_t MemoryMappedFile::Read48() {
    const uint8_t* ptr = static_cast<const uint8_t*>(mMappedView) + mCursor;

    constexpr int byte_size = 6;
    constexpr int first = 0;
    constexpr int second = 8;
    constexpr int third = 16;
    constexpr int fourth = 24;
    constexpr int fifth = 32;
    constexpr int sixth = 40;

    const uint64_t value = (static_cast<uint64_t>(ptr[0]) << sixth) |
                           (static_cast<uint64_t>(ptr[1]) << fifth) |
                           (static_cast<uint64_t>(ptr[2]) << fourth) |
                           (static_cast<uint64_t>(ptr[3]) << third) |
                           (static_cast<uint64_t>(ptr[4]) << second) |
                           (static_cast<uint64_t>(ptr[5]) << first);
    mCursor += byte_size;
    return value;
}

// OS-specific implementations

#ifdef __WINDOWS_OS__

bool MemoryMappedFile::osOpen(const std::filesystem::path& filename) {
    mFileHandle =
        ::CreateFileW(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    return mFileHandle != INVALID_HANDLE_VALUE;
}

void MemoryMappedFile::osClose() {
    if (static_cast<bool>(mFileHandle) && mFileHandle != INVALID_HANDLE_VALUE) {
        ::CloseHandle(mFileHandle);
        mFileHandle = nullptr;
    }
}

bool MemoryMappedFile::osMap(uint64_t offset, size_t mapped_bytes) {
    if (mapped_bytes == static_cast<size_t>(MapRange::WholeFile)) {
        mapped_bytes = static_cast<size_t>(mFilesize - offset);
    }

    mMappingHandle = ::CreateFileMappingW(mFileHandle, nullptr, PAGE_READONLY,
                                          0, 0, nullptr);
    if (!static_cast<bool>(mMappingHandle)) {
        return false;
    }

    const auto offset_high = static_cast<DWORD>(offset >> 32);
    const auto offset_low = static_cast<DWORD>(offset & 0xFFFFFFFF);

    mMappedView = ::MapViewOfFile(mMappingHandle, FILE_MAP_READ, offset_high,
                                  offset_low, mapped_bytes);

    if (!static_cast<bool>(mMappedView)) {
        ::CloseHandle(mMappingHandle);
        mMappingHandle = nullptr;
        return false;
    }

    mMappedBytes = mapped_bytes;
    return true;
}

void MemoryMappedFile::osUnmap() {
    if (static_cast<bool>(mMappedView)) {
        ::UnmapViewOfFile(mMappedView);
        mMappedView = nullptr;
    }
    if (static_cast<bool>(mMappingHandle)) {
        ::CloseHandle(mMappingHandle);
        mMappingHandle = nullptr;
    }
}

uint64_t MemoryMappedFile::osGetFileSize() const {
    LARGE_INTEGER size;
    if (::GetFileSizeEx(mFileHandle, &size)) {
        return static_cast<uint64_t>(size.QuadPart);
    }
    return 0;
}

uint32_t MemoryMappedFile::getPageSize() {
    SYSTEM_INFO info;
    ::GetSystemInfo(&info);
    return info.dwAllocationGranularity;
}

#else  // Unix

bool MemoryMappedFile::osOpen(const std::filesystem::path& filename) {
    mFileHandle = ::open(filename.c_str(), O_RDONLY);
    return mFileHandle != -1;
}

void MemoryMappedFile::osClose() {
    if (mFileHandle != -1) {
        ::close(mFileHandle);
        mFileHandle = -1;
    }
}

bool MemoryMappedFile::osMap(uint64_t offset, size_t mapped_bytes) {
    if (mapped_bytes == static_cast<size_t>(MapRange::WholeFile)) {
        mapped_bytes = static_cast<size_t>(mFilesize - offset);
    }

    int flags = MAP_SHARED;
    #ifdef MAP_POPULATE
    if (mHint == CacheHint::SequentialScan) {
        flags |= MAP_POPULATE;
    }
    #endif

    mMappedView =
        ::mmap(nullptr, mapped_bytes, PROT_READ, flags, mFileHandle, offset);

    if (static_cast<bool>(mMappedView) == MAP_FAILED) {
        mMappedView = nullptr;
        return false;
    }

    #ifdef MADV_SEQUENTIAL
    if (mHint == CacheHint::SequentialScan) {
        ::madvise(mMappedView, mapped_bytes, MADV_SEQUENTIAL);
    }
    #endif

    mMappedBytes = mapped_bytes;
    return true;
}

void MemoryMappedFile::osUnmap() {
    if (static_cast<bool>(mMappedView)) {
        ::munmap(mMappedView, mMappedBytes);
        mMappedView = nullptr;
    }
}

uint64_t MemoryMappedFile::osGetFileSize() const {
    struct stat st;
    if (::fstat(mFileHandle, &st) == 0) {
        return static_cast<uint64_t>(st.st_size);
    }
    return 0;
}

uint32_t MemoryMappedFile::getPageSize() {
    return static_cast<uint32_t>(::sysconf(_SC_PAGESIZE));
}

#endif