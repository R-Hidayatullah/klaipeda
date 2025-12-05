#if !defined(UTILS_HPP)
#define UTILS_HPP

#include <vector>
#include <cstdint>
#include <string>

void printHexViewer(const std::vector<uint8_t> &data);
void logInfo(const std::string &msg);
void logWarn(const std::string &msg);
void logError(const std::string &msg);

#endif // UTILS_HPP
