#include "ipf/ipf_reader.hpp"
#include "ipf/binary_reader.hpp"
#include "ipf/decrypt.hpp"
#include "ipf/decompress.hpp"
#include "ipf/utils.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

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

bool readIpfRootFromPath(const std::string &path, IPFRoot &out)
{
    BinaryReader br(path);
    if (!br.ok())
    {
        out.warnings.push_back("Failed to open file: " + path);
        for (auto &e : br.errors())
            out.warnings.push_back(e.message);
        return false;
    }

    // Seek to end-24 to read header
    if (!br.seek(HEADER_LOCATION, std::ios::end))
    {
        out.warnings.push_back("Seek to header failed");
        for (auto &e : br.errors())
            out.warnings.push_back(e.message);
        return false;
    }

    // Read header fields in little-endian
    if (!br.readLe<uint16_t>(out.header.file_count))
    {
        goto err;
    }
    if (!br.readLe<uint32_t>(out.header.file_table_pointer))
    {
        goto err;
    }
    if (!br.readLe<uint16_t>(out.header.padding))
    {
        goto err;
    }
    if (!br.readLe<uint32_t>(out.header.header_pointer))
    {
        goto err;
    }
    if (!br.readLe<uint32_t>(out.header.magic))
    {
        goto err;
    }
    if (!br.readLe<uint32_t>(out.header.version_to_patch))
    {
        goto err;
    }
    if (!br.readLe<uint32_t>(out.header.new_version))
    {
        goto err;
    }

    if (out.header.magic != MAGIC_NUMBER)
    {
        std::ostringstream oss;
        oss << "Header magic mismatch: got 0x" << std::hex << out.header.magic << " expected 0x" << MAGIC_NUMBER;
        out.warnings.push_back(oss.str());
    }

    // Seek to file table pointer
    if (!br.seek(static_cast<std::streamoff>(out.header.file_table_pointer), std::ios::beg))
    {
        out.warnings.push_back("Failed to seek to file_table_pointer");
        goto err;
    }

    out.file_table.clear();
    out.file_table.reserve(out.header.file_count);

    for (uint32_t i = 0; i < out.header.file_count; ++i)
    {
        IPFFileTable f;
        if (!br.readLe<uint16_t>(f.directory_name_length))
        {
            goto err;
        }
        if (!br.readLe<uint32_t>(f.crc32))
        {
            goto err;
        }
        if (!br.readLe<uint32_t>(f.file_size_compressed))
        {
            goto err;
        }
        if (!br.readLe<uint32_t>(f.file_size_uncompressed))
        {
            goto err;
        }
        if (!br.readLe<uint32_t>(f.file_pointer))
        {
            goto err;
        }
        if (!br.readLe<uint16_t>(f.container_name_length))
        {
            goto err;
        }

        // read container_name
        std::vector<uint8_t> tmp;
        if (!br.readBytes(tmp, f.container_name_length))
        {
            goto err;
        }
        f.container_name.assign(tmp.begin(), tmp.end());

        // read directory_name
        if (!br.readBytes(tmp, f.directory_name_length))
        {
            goto err;
        }
        f.directory_name.assign(tmp.begin(), tmp.end());

        f.file_path = path;

        // Prepend container stem like in Rust
        // get stem from container_name (strip extension)
        auto dotpos = f.container_name.find_last_of('.');
        std::string stem = (dotpos == std::string::npos) ? f.container_name : f.container_name.substr(0, dotpos);
        f.directory_name = stem + "/" + f.directory_name;

        out.file_table.push_back(std::move(f));
    }

    return true;

err:
    for (auto &e : br.errors())
        out.warnings.push_back(e.message);
    return false;
}

bool extractFileData(const IPFFileTable &ent, std::vector<uint8_t> &out, std::string &err)
{
    BinaryReader br;
    if (!br.open(ent.file_path))
    {
        err = "Failed to open " + ent.file_path;
        return false;
    }

    if (!br.seek(static_cast<std::streamoff>(ent.file_pointer), std::ios::beg))
    {
        err = "Seek to file_pointer failed";
        return false;
    }

    if (ent.file_size_compressed == 0)
    {
        out.clear();
        return true;
    }

    if (!br.readBytes(out, ent.file_size_compressed))
    {
        err = "Failed to read compressed bytes";
        return false;
    }

    if (!ent.shouldSkipDecompression())
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
