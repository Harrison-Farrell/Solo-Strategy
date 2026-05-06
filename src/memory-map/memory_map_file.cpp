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

    if (!OSOpen(filename)) {
        return false;
    }

    m_filename = filename;
    m_Hint = hint;
    m_file_size = OSGetFileSize();

    if (m_file_size == 0) {
        OSClose();
        return false;
    }

    if (!OSMap(0, mapped_bytes)) {
        OSClose();
        return false;
    }

    return true;
}

void MemoryMappedFile::Close() {
    OSUnmap();
    OSClose();

    m_filename.clear();
    m_file_size = 0;
    m_MappedBytes = 0;
    m_cursor = 0;
}

bool MemoryMappedFile::Remap(uint64_t offset, size_t mapped_bytes) {
    if (!IsValid()) {
        return false;
    }

    if (offset + mapped_bytes > m_file_size &&
        mapped_bytes != static_cast<size_t>(MapRange::WholeFile)) {
        return false;
    }

    OSUnmap();
    return OSMap(offset, mapped_bytes);
}

uint8_t MemoryMappedFile::operator[](size_t offset) const {
    return static_cast<const uint8_t*>(m_MappedView)[offset];
}

uint8_t MemoryMappedFile::At(size_t offset) const {
    if (offset >= m_MappedBytes) {
        throw std::out_of_range("MemoryMappedFile::at() : offset out of range");
    }
    return operator[](offset);
}

const uint8_t* MemoryMappedFile::GetData() const {
    return static_cast<const uint8_t*>(m_MappedView);
}

const uint8_t* MemoryMappedFile::Begin() const { return GetData(); }

const uint8_t* MemoryMappedFile::End() const {
    return GetData() + m_MappedBytes;
}

bool MemoryMappedFile::IsValid() const { return m_MappedView != nullptr; }

uint64_t MemoryMappedFile::Size() const { return m_file_size; }

size_t MemoryMappedFile::MappedSize() const { return m_MappedBytes; }

uint64_t MemoryMappedFile::Read48() {
    const uint8_t* ptr = static_cast<const uint8_t*>(m_MappedView) + m_cursor;

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
    m_cursor += byte_size;
    return value;
}

// OS-specific implementations

#ifdef __WINDOWS_OS__

bool MemoryMappedFile::OSOpen(const std::filesystem::path& filename) {
    m_FileHandle =
        ::CreateFileW(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    return m_FileHandle != INVALID_HANDLE_VALUE;
}

void MemoryMappedFile::OSClose() {
    if (static_cast<bool>(m_FileHandle) &&
        m_FileHandle != INVALID_HANDLE_VALUE) {
        ::CloseHandle(m_FileHandle);
        m_FileHandle = nullptr;
    }
}

bool MemoryMappedFile::OSMap(uint64_t offset, size_t mapped_bytes) {
    if (mapped_bytes == static_cast<size_t>(MapRange::WholeFile)) {
        mapped_bytes = static_cast<size_t>(m_file_size - offset);
    }

    m_MappingHandle = ::CreateFileMappingW(m_FileHandle, nullptr, PAGE_READONLY,
                                           0, 0, nullptr);
    if (!static_cast<bool>(m_MappingHandle)) {
        return false;
    }

    const auto offset_high = static_cast<DWORD>(offset >> 32);
    const auto offset_low = static_cast<DWORD>(offset & 0xFFFFFFFF);

    m_MappedView = ::MapViewOfFile(m_MappingHandle, FILE_MAP_READ, offset_high,
                                   offset_low, mapped_bytes);

    if (!static_cast<bool>(m_MappedView)) {
        ::CloseHandle(m_MappingHandle);
        m_MappingHandle = nullptr;
        return false;
    }

    m_MappedBytes = mapped_bytes;
    return true;
}

void MemoryMappedFile::OSUnmap() {
    if (static_cast<bool>(m_MappedView)) {
        ::UnmapViewOfFile(m_MappedView);
        m_MappedView = nullptr;
    }
    if (static_cast<bool>(m_MappingHandle)) {
        ::CloseHandle(m_MappingHandle);
        m_MappingHandle = nullptr;
    }
}

uint64_t MemoryMappedFile::OSGetFileSize() const {
    LARGE_INTEGER size;
    if (::GetFileSizeEx(m_FileHandle, &size)) {
        return static_cast<uint64_t>(size.QuadPart);
    }
    return 0;
}

uint32_t MemoryMappedFile::GetPageSize() {
    SYSTEM_INFO info;
    ::GetSystemInfo(&info);
    return info.dwAllocationGranularity;
}

#else  // Unix

bool MemoryMappedFile::OSOpen(const std::filesystem::path& filename) {
    m_FileHandle = ::open(filename.c_str(), O_RDONLY);
    return m_FileHandle != -1;
}

void MemoryMappedFile::OSClose() {
    if (m_FileHandle != -1) {
        ::close(m_FileHandle);
        m_FileHandle = -1;
    }
}

bool MemoryMappedFile::OSMap(uint64_t offset, size_t mapped_bytes) {
    if (mapped_bytes == static_cast<size_t>(MapRange::WholeFile)) {
        mapped_bytes = static_cast<size_t>(m_file_size - offset);
    }

    int flags = MAP_SHARED;
    #ifdef MAP_POPULATE
    if (m_Hint == CacheHint::SequentialScan) {
        flags |= MAP_POPULATE;
    }
    #endif

    m_MappedView =
        ::mmap(nullptr, mapped_bytes, PROT_READ, flags, m_FileHandle, offset);

    if (m_MappedView == MAP_FAILED) {
        m_MappedView = nullptr;
        return false;
    }

    #ifdef MADV_SEQUENTIAL
    if (m_Hint == CacheHint::SequentialScan) {
        ::madvise(m_MappedView, mapped_bytes, MADV_SEQUENTIAL);
    }
    #endif

    m_MappedBytes = mapped_bytes;
    return true;
}

void MemoryMappedFile::OSUnmap() {
    if (static_cast<bool>(m_MappedView)) {
        ::munmap(m_MappedView, m_MappedBytes);
        m_MappedView = nullptr;
    }
}

uint64_t MemoryMappedFile::OSGetFileSize() const {
    struct stat st;
    if (::fstat(m_FileHandle, &st) == 0) {
        return static_cast<uint64_t>(st.st_size);
    }
    return 0;
}

uint32_t MemoryMappedFile::GetPageSize() {
    return static_cast<uint32_t>(::sysconf(_SC_PAGESIZE));
}

#endif