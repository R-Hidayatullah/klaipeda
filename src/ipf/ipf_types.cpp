#include "ipf/ipf_types.hpp"
#include "ipf/binary_reader.hpp"
#include "ipf/decompress.hpp"
#include "ipf/decrypt.hpp"

// Helper: case-insensitive strcmp for portability
int strcasecmp(const char *a, const char *b)
{
    while (*a && *b)
    {
        char ca = *a++;
        char cb = *b++;
        if (ca >= 'A' && ca <= 'Z')
            ca += 'a' - 'A';
        if (cb >= 'A' && cb <= 'Z')
            cb += 'a' - 'A';
        if (ca != cb)
            return (unsigned char)ca - (unsigned char)cb;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

bool IPFFileTable::shouldSkipDecompression() const
{
    static const std::vector<std::string> ignored = {".fsb", ".jpg", ".mp3"};
    auto dot = directory_name.find_last_of('.');
    if (dot == std::string::npos)
        return false;
    std::string ext = directory_name.substr(dot);
    for (auto &e : ignored)
        if (strcasecmp(e.c_str(), ext.c_str()) == 0)
            return true;
    return false;
}

bool IPFFileTable::extractFileData(std::vector<uint8_t> &out, std::string &err)
{
    BinaryReader br;
    if (!br.open(file_path))
    {
        err = "Failed to open " + file_path;
        return false;
    }

    if (!br.seek(static_cast<std::streamoff>(file_pointer), std::ios::beg))
    {
        err = "Seek to file_pointer failed";
        return false;
    }

    if (file_size_compressed == 0)
    {
        out.clear();
        return true;
    }

    if (!br.readBytes(out, file_size_compressed))
    {
        err = "Failed to read compressed bytes";
        return false;
    }

    if (!shouldSkipDecompression())
    {
        // decrypt in-place
        decryptInplace(out);

        // decompress
        std::vector<uint8_t> decompressed;
        if (!decompressZlib(out, decompressed, err))
        {
            return false;
        }
        out.swap(decompressed);
    }

    return true;
}
