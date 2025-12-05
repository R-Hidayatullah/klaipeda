#if !defined(IPF_READER_HPP)
#define IPF_READER_HPP
#include "ipf/ipf_types.hpp"
#include <string>

bool readIpfRootFromPath(const std::string &path, IPFRoot &out);
bool extractFileData(const IPFFileTable &ent, std::vector<uint8_t> &out, std::string &err);

#endif // IPF_READER_HPP
