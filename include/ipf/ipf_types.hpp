#if !defined(IPF_TYPES_HPP)
#define IPF_TYPES_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <map>

static const uint32_t MAGIC_NUMBER = 0x06054B50;
static const int HEADER_LOCATION = -24;

struct IPFHeader
{
    uint16_t file_count;
    uint32_t file_table_pointer;
    uint16_t padding;
    uint32_t header_pointer;
    uint32_t magic;
    uint32_t version_to_patch;
    uint32_t new_version;
};

struct IPFFileTable
{
    uint16_t directory_name_length;
    uint32_t crc32;
    uint32_t file_size_compressed;
    uint32_t file_size_uncompressed;
    uint32_t file_pointer;
    uint16_t container_name_length;

    std::string container_name; // from raw bytes
    std::string directory_name; // from raw bytes
    std::string file_path;      // full path to container .ipf

    bool shouldSkipDecompression() const;
};

struct IPFRoot
{
    IPFHeader header;
    std::vector<IPFFileTable> file_table;
    std::vector<std::string> warnings;
};

#endif // IPF_TYPES_HPP
