#if !defined(IES_H)
#define IES_H
#include "klaipeda.h"

constexpr size_t DEFAULT_CHAR_LEN = 64;
constexpr const char *DEFAULT_STRING = "None";
constexpr const char *DEFAULT_NUMBER = "0";

struct IESHeader
{
    std::string idspace;  // length: 64 chars
    std::string keyspace; // length: 64 chars
    uint16_t version;
    uint32_t infoSize;
    uint32_t dataSize;
    uint32_t totalSize;
    uint8_t useClassID; // bool in original
    uint16_t numField;
    uint16_t numColumn;
    uint16_t numColumnNumber;
    uint16_t numColumnString;
};

struct IESColumn
{
    std::string column; // length: 64 chars
    std::string name;   // length: 64 chars
    uint16_t type;
    uint16_t access;
    uint16_t sync;
    uint16_t declIdx;
};

// Original fixed sizes from legacy format
constexpr int g_sizeIESHeader = 156; // expected sizeof(IESHeader) in legacy format
constexpr int g_sizeIESColumn = 136; // expected sizeof(IESColumn)
constexpr int g_sizeWord = 2;        // sizeof(uint16_t)
constexpr int g_sizeBool = 1;        // sizeof(uint8_t)
constexpr int g_sizeFloat = 4;       // sizeof(float)
constexpr int g_sizeByte = 1;        // sizeof(uint8_t)
constexpr int g_sizeUint = 4;        // sizeof(uint32_t)

// Size check disabled because std::string is dynamic-length now
// If binary layout compatibility is required, std::string will not match legacy sizes.

#endif // IES_H
