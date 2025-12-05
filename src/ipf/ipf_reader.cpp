#include "ipf/ipf_reader.hpp"
#include "ipf/binary_reader.hpp"
#include "ipf/decrypt.hpp"
#include "ipf/decompress.hpp"
#include "ipf/utils.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

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

    auto fail = [&out, &br](const std::string &msg)
    {
        out.warnings.push_back(msg);
        for (auto &e : br.errors())
            out.warnings.push_back(e.message);
        return false;
    };

    // Seek to end-24 to read header
    if (!br.seek(HEADER_LOCATION, std::ios::end))
        return fail("Seek to header failed");

    // Read header fields in little-endian
    if (!br.readLe<uint16_t>(out.header.file_count))
        return fail("Failed to read file_count");
    if (!br.readLe<uint32_t>(out.header.file_table_pointer))
        return fail("Failed to read file_table_pointer");
    if (!br.readLe<uint16_t>(out.header.padding))
        return fail("Failed to read padding");
    if (!br.readLe<uint32_t>(out.header.header_pointer))
        return fail("Failed to read header_pointer");
    if (!br.readLe<uint32_t>(out.header.magic))
        return fail("Failed to read magic");
    if (!br.readLe<uint32_t>(out.header.version_to_patch))
        return fail("Failed to read version_to_patch");
    if (!br.readLe<uint32_t>(out.header.new_version))
        return fail("Failed to read new_version");

    if (out.header.magic != MAGIC_NUMBER)
    {
        std::ostringstream oss;
        oss << "Header magic mismatch: got 0x" << std::hex << out.header.magic
            << " expected 0x" << MAGIC_NUMBER;
        out.warnings.push_back(oss.str());
    }

    // Seek to file table pointer
    if (!br.seek(static_cast<std::streamoff>(out.header.file_table_pointer), std::ios::beg))
        return fail("Failed to seek to file_table_pointer");

    out.file_table.clear();
    out.file_table.reserve(out.header.file_count);

    for (uint32_t i = 0; i < out.header.file_count; ++i)
    {
        IPFFileTable f;

        if (!br.readLe<uint16_t>(f.directory_name_length))
            return fail("Failed to read directory_name_length");
        if (!br.readLe<uint32_t>(f.crc32))
            return fail("Failed to read crc32");
        if (!br.readLe<uint32_t>(f.file_size_compressed))
            return fail("Failed to read file_size_compressed");
        if (!br.readLe<uint32_t>(f.file_size_uncompressed))
            return fail("Failed to read file_size_uncompressed");
        if (!br.readLe<uint32_t>(f.file_pointer))
            return fail("Failed to read file_pointer");
        if (!br.readLe<uint16_t>(f.container_name_length))
            return fail("Failed to read container_name_length");

        std::vector<uint8_t> tmp;

        if (!br.readBytes(tmp, f.container_name_length))
            return fail("Failed to read container_name");
        f.container_name.assign(tmp.begin(), tmp.end());

        if (!br.readBytes(tmp, f.directory_name_length))
            return fail("Failed to read directory_name");
        f.directory_name.assign(tmp.begin(), tmp.end());

        f.file_path = path;

        // Prepend container stem
        auto dotpos = f.container_name.find_last_of('.');
        std::string stem = (dotpos == std::string::npos) ? f.container_name : f.container_name.substr(0, dotpos);
        f.directory_name = stem + "/" + f.directory_name;

        out.file_table.push_back(std::move(f));
    }

    return true;
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
