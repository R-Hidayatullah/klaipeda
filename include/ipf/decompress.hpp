#if !defined(DECOMPRESS_HPP)
#define DECOMPRESS_HPP
#include <vector>
#include <string>
#include <stdint.h>

bool decompressZlib(const std::vector<uint8_t> &in, std::vector<uint8_t> &out, std::string &err);

#endif // DECOMPRESS_HPP
