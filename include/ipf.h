#if !defined(IPF_H)
#define IPF_H
#include "klaipeda.h"

#pragma pack(push, 1) // Ensure structures are packed without padding

/**
 * Structure representing the footer of an IPF file.
 * This footer is located at the end of the file and contains metadata
 * about the stored files and their locations.
 */
struct IPF_Footer
{
    uint16_t file_count;         // Number of files in the archive
    uint32_t file_table_pointer; // Offset to the file table in the archive
    uint16_t padding;            // Unused padding (likely for alignment)
    uint32_t footer_pointer;     // Offset to this footer structure
    uint32_t magic;              // Unique identifier (magic number) for validation
    uint32_t version_to_patch;   // Version number before patching
    uint32_t new_version;        // Version number after patching
};

/**
 * Structure representing an entry in the IPF file table.
 * Each entry contains metadata and associated strings for
 * the container name and directory name.
 */
typedef struct
{
    uint16_t directory_name_length;  // Length of the directory name string
    uint32_t crc32;                  // CRC32 checksum for integrity verification
    uint32_t file_size_compressed;   // File size after compression (in bytes)
    uint32_t file_size_uncompressed; // Original file size before compression (in bytes)
    uint32_t file_pointer;           // Offset to the actual file data in the archive
    uint16_t container_name_length;  // Length of the container name string
    std::string container_name;      // Pointer to the container name string
    std::string directory_name;      // Pointer to the directory name string
} IPF_FileTable;

/**
 * Root structure for the IPF archive, containing the footer and file table.
 * This structure is used to manage the archive data in memory.
 */
typedef struct
{
    IPF_Footer ipf_footer;                     // Footer structure containing archive metadata
    std::vector<IPF_FileTable> ipf_file_table; // Pointer to the dynamically allocated file table
} IPF_Root;

#pragma pack(pop) // Restore default alignment

// Template function to read binary data into a given type
template <typename T>
void read(std::ifstream &file, T &data)
{
    file.read(reinterpret_cast<char *>(&data), sizeof(T));
}

// Function to read a string with a given length
void read(std::ifstream &file, std::string &data, uint16_t length)
{
    data.resize(length);
    file.read(&data[0], length);
}

// Specialization for std::vector<T>
template <typename T>
void read(std::ifstream &file, std::vector<T> &data, size_t count)
{
    data.resize(count);
    file.read(reinterpret_cast<char *>(data.data()), sizeof(T) * count);
}

// Specialization for std::array<T, N>
template <typename T, size_t N>
void read(std::ifstream &file, std::array<T, N> &data)
{
    file.read(reinterpret_cast<char *>(data.data()), sizeof(T) * N);
}

bool parse_ipf(const std::string &filename, IPF_Root &ipf_root)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open file: " << filename << '\n';
        return false;
    }

    std::cout << "Opened file: " << filename << '\n';

    // Read footer from the end of the file
    file.seekg(-static_cast<int>(sizeof(IPF_Footer)), std::ios::end);
    read(file, ipf_root.ipf_footer);

    std::cout << "IPF Footer:\n";
    std::cout << "  File Count: " << ipf_root.ipf_footer.file_count << '\n';
    std::cout << "  File Table Pointer: " << std::hex << ipf_root.ipf_footer.file_table_pointer << std::dec << '\n';
    std::cout << "  Footer Pointer: " << std::hex << ipf_root.ipf_footer.footer_pointer << std::dec << '\n';
    std::cout << "  Magic: 0x" << std::hex << ipf_root.ipf_footer.magic << std::dec << '\n';
    std::cout << "  Version to Patch: " << ipf_root.ipf_footer.version_to_patch << '\n';
    std::cout << "  New Version: " << ipf_root.ipf_footer.new_version << '\n';

    // Read file table
    file.seekg(ipf_root.ipf_footer.file_table_pointer, std::ios::beg);
    ipf_root.ipf_file_table.resize(ipf_root.ipf_footer.file_count);

    std::cout << "\nReading File Table (" << ipf_root.ipf_footer.file_count << " entries):\n";

    for (size_t i = 0; i < ipf_root.ipf_file_table.size(); ++i)
    {
        auto &entry = ipf_root.ipf_file_table[i];

        read(file, entry.directory_name_length);
        read(file, entry.crc32);
        read(file, entry.file_size_compressed);
        read(file, entry.file_size_uncompressed);
        read(file, entry.file_pointer);
        read(file, entry.container_name_length);
        read(file, entry.container_name, entry.container_name_length);
        read(file, entry.directory_name, entry.directory_name_length);

        if (i < 3)
        {
            std::cout << "\nEntry " << i + 1 << ":\n";
            std::cout << "  Directory Name Length: " << entry.directory_name_length << '\n';
            std::cout << "  CRC32: 0x" << std::hex << entry.crc32 << std::dec << '\n';
            std::cout << "  Compressed Size: " << entry.file_size_compressed << " bytes\n";
            std::cout << "  Uncompressed Size: " << entry.file_size_uncompressed << " bytes\n";
            std::cout << "  File Pointer: " << std::hex << entry.file_pointer << std::dec << '\n';
            std::cout << "  Container Name Length: " << entry.container_name_length << '\n';
            std::cout << "  Container Name: " << entry.container_name << '\n';
            std::cout << "  Directory Name: " << entry.directory_name << '\n';
        }
    }

    std::cout << "\nFinished parsing IPF file.\n";
    return true;
}

#endif // IPF_H
