#include "ipf/binary_reader.hpp"
#include <sstream>
#include <cstring>
#include <iostream>

BinaryReader::BinaryReader(const std::string &path) { open(path); }

bool BinaryReader::open(const std::string &path)
{
    file_path = path;
    file.open(path, std::ios::binary);
    file_good = static_cast<bool>(file);
    if (!file_good)
        addError("Failed to open: " + path);
    return file_good;
}

std::streampos BinaryReader::tell()
{
    if (!file_good)
        return std::streampos(-1); // indicate invalid position

    std::streampos pos = file.tellg();
    if (pos == std::streampos(-1))
    {
        addError("tell() failed: stream in bad state");
    }
    return pos;
}

void BinaryReader::addError(const std::string &msg)
{
    BinaryError e;
    e.message = msg;
    e.position = tell();
    error_list.push_back(e);
    file_good = false;
}

bool BinaryReader::seek(std::streamoff off, std::ios::seekdir dir)
{
    if (!file)
    {
        addError("Seek on closed file");
        return false;
    }
    file.clear();
    file.seekg(off, dir);
    if (!file)
    {
        addError("Seek failed");
        return false;
    }
    return true;
}

template <typename T>
bool BinaryReader::readLe(T &out)
{
    static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable");
    if (!file)
    {
        addError("Read on closed file");
        return false;
    }
    file.read(reinterpret_cast<char *>(&out), sizeof(T));
    if (!file)
    {
        addError("Failed to read bytes");
        return false;
    }
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    // Convert from little-endian to host if big-endian system
    // (most desktop systems are little-endian; still provided)
    T res = 0;
    uint8_t *src = reinterpret_cast<uint8_t *>(&out);
    uint8_t *dst = reinterpret_cast<uint8_t *>(&res);
    for (size_t i = 0; i < sizeof(T); ++i)
        dst[i] = src[sizeof(T) - 1 - i];
    out = res;
#endif
    return true;
}

// Explicit template instantiations we will use
template bool BinaryReader::readLe<uint16_t>(uint16_t &);
template bool BinaryReader::readLe<uint32_t>(uint32_t &);
template bool BinaryReader::readLe<int32_t>(int32_t &);
template bool BinaryReader::readLe<int64_t>(int64_t &);

bool BinaryReader::readBytes(std::vector<uint8_t> &out, size_t count)
{
    out.resize(count);
    if (!file)
    {
        addError("Read bytes on closed file");
        return false;
    }
    file.read(reinterpret_cast<char *>(out.data()), count);
    if (!file)
    {
        addError("Failed to read expected bytes (" + std::to_string(count) + ")");
        return false;
    }
    return true;
}
