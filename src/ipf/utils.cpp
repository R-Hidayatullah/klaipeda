#include "ipf/utils.hpp"

#include <iostream>
#include <iomanip>
#include <cctype>

void printHexViewer(const std::vector<uint8_t> &data)
{
    const size_t BYTES_PER_LINE = 16;
    for (size_t i = 0; i < data.size(); i += BYTES_PER_LINE)
    {
        std::cout << std::setw(8) << std::setfill('0') << i << "  ";
        for (size_t j = 0; j < BYTES_PER_LINE; ++j)
        {
            if (i + j < data.size())
                std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                          << (int)data[i + j] << " ";
            else
                std::cout << "   ";
        }
        std::cout << " ";
        for (size_t j = 0; j < BYTES_PER_LINE && i + j < data.size(); ++j)
        {
            unsigned char c = data[i + j];
            if (std::isprint(c) || c == ' ')
                std::cout << (char)c;
            else
                std::cout << '.';
        }
        std::cout << std::dec << "\n";
    }
}

void logInfo(const std::string &msg)
{
    std::cout << "[INFO] " << msg << "\n";
}
void logWarn(const std::string &msg)
{
    std::cout << "[WARN] " << msg << "\n";
}
void logError(const std::string &msg)
{
    std::cerr << "[ERROR] " << msg << "\n";
}
