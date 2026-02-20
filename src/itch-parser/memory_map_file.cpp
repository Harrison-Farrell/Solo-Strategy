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

#include "itch-parser/memory_map_file.h"

#include <cstdio>
#include <stdexcept>

#ifdef __WINDOWS_OS__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

MemoryMappedFile::MemoryMappedFile() = default;

MemoryMappedFile::MemoryMappedFile(const std::filesystem::path& filename, size_t mappedBytes,
                                   CacheHint hint) {
    if (!open(filename, mappedBytes, hint)) {
        throw std::runtime_error("MemoryMappedFile::MemoryMappedFile() : failed to open file");
    }
}

MemoryMappedFile::~MemoryMappedFile() {
    close();
}

bool MemoryMappedFile::open(const std::filesystem::path& filename, size_t mappedBytes,
                            CacheHint hint) {
    close();

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

    if (!osMap(0, mappedBytes)) {
        osClose();
        return false;
    }

    return true;
}

void MemoryMappedFile::close() {
    osUnmap();
    osClose();

    mFilename.clear();
    mFilesize = 0;
    mMappedBytes = 0;
    mCursor = 0;
}

bool MemoryMappedFile::remap(uint64_t offset, size_t mappedBytes) {
    if (!isValid()) {
        return false;
    }

    if (offset + mappedBytes > mFilesize && mappedBytes != WholeFile) {
        return false;
    }

    osUnmap();
    return osMap(offset, mappedBytes);
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

const uint8_t* MemoryMappedFile::begin() const {
    return getData();
}

const uint8_t* MemoryMappedFile::end() const {
    return getData() + mMappedBytes;
}

bool MemoryMappedFile::isValid() const {
    return mMappedView != nullptr;
}

uint64_t MemoryMappedFile::size() const {
    return mFilesize;
}

size_t MemoryMappedFile::mappedSize() const {
    return mMappedBytes;
}

// OS-specific implementations

#ifdef __WINDOWS_OS__

bool MemoryMappedFile::osOpen(const std::filesystem::path& filename) {
    mFileHandle = ::CreateFileW(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    return mFileHandle != INVALID_HANDLE_VALUE;
}

void MemoryMappedFile::osClose() {
    if (mFileHandle && mFileHandle != INVALID_HANDLE_VALUE) {
        ::CloseHandle(mFileHandle);
        mFileHandle = nullptr;
    }
}

bool MemoryMappedFile::osMap(uint64_t offset, size_t mappedBytes) {
    if (mappedBytes == WholeFile) {
        mappedBytes = static_cast<size_t>(mFilesize - offset);
    }

    mMappingHandle = ::CreateFileMappingW(mFileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (!mMappingHandle) {
        return false;
    }

    const DWORD offsetHigh = static_cast<DWORD>(offset >> 32);
    const DWORD offsetLow = static_cast<DWORD>(offset & 0xFFFFFFFF);

    mMappedView = ::MapViewOfFile(mMappingHandle, FILE_MAP_READ, offsetHigh, offsetLow, mappedBytes);

    if (!mMappedView) {
        ::CloseHandle(mMappingHandle);
        mMappingHandle = nullptr;
        return false;
    }

    mMappedBytes = mappedBytes;
    return true;
}

void MemoryMappedFile::osUnmap() {
    if (mMappedView) {
        ::UnmapViewOfFile(mMappedView);
        mMappedView = nullptr;
    }
    if (mMappingHandle) {
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
    SYSTEM_INFO si;
    ::GetSystemInfo(&si);
    return si.dwAllocationGranularity;
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

bool MemoryMappedFile::osMap(uint64_t offset, size_t mappedBytes) {
    if (mappedBytes == WholeFile) {
        mappedBytes = static_cast<size_t>(mFilesize - offset);
    }

    int flags = MAP_SHARED;
#ifdef MAP_POPULATE
    if (mHint == Sequential) {
        flags |= MAP_POPULATE;
    }
#endif

    mMappedView = ::mmap(nullptr, mappedBytes, PROT_READ, flags, mFileHandle, offset);

    if (mMappedView == MAP_FAILED) {
        mMappedView = nullptr;
        return false;
    }

#ifdef MADV_SEQUENTIAL
    if (mHint == Sequential) {
        ::madvise(mMappedView, mappedBytes, MADV_SEQUENTIAL);
    }
#endif

    mMappedBytes = mappedBytes;
    return true;
}

void MemoryMappedFile::osUnmap() {
    if (mMappedView) {
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