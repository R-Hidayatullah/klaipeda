#if !defined(BINARY_READER_HPP)
#define BINARY_READER_HPP
#include <fstream>
#include <string>
#include <vector>
#include <stdint.h>

struct BinaryError
{
    std::string message;
    std::streampos position;
};

class BinaryReader
{
public:
    BinaryReader() = default;
    explicit BinaryReader(const std::string &path);

    bool open(const std::string &path);
    bool ok() const { return file_good; }
    const std::vector<BinaryError> &errors() const { return error_list; }

    template <typename T>
    bool readLe(T &out); // little-endian read

    bool readBytes(std::vector<uint8_t> &out, size_t count);
    bool seek(std::streamoff off, std::ios::seekdir dir);
    std::streampos tell();

    const std::string &path() const { return file_path; }

    void addError(const std::string &msg);

private:
    std::ifstream file;
    bool file_good = false;
    std::vector<BinaryError> error_list;
    std::string file_path;
};

#endif // BINARY_READER_HPP
