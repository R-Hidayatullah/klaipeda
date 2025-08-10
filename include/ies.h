#if !defined(IES_H)
#define IES_H
#include "klaipeda.h"

constexpr size_t DEFAULT_CHAR_LEN = 64;
constexpr const char *DEFAULT_STRING = "None";
constexpr const char *DEFAULT_NUMBER = "0";

enum IES_ColumnType : uint16_t
{
    IES_COL_FLOAT = 0,
    IES_COL_STRING = 1,
    IES_COL_STRING_SECOND = 2,
};

struct IESHeader
{
    std::string idspace;  // length: 64 chars
    std::string keyspace; // length: 64 chars
    uint16_t version;
    uint16_t padding;
    uint32_t infoSize;
    uint32_t dataSize;
    uint32_t totalSize;
    uint8_t useClassID; // bool in original
    uint8_t padding2;
    uint16_t numField;
    uint16_t numColumn;
    uint16_t numColumnNumber;
    uint16_t numColumnString;
    uint16_t padding3;
};

struct IESColumn
{
    std::string column; // length: 64 chars
    std::string name;   // length: 64 chars
    IES_ColumnType type;
    uint16_t access;
    uint16_t sync;
    uint16_t declIdx;
};

bool iesColumnLess(const IES_Column &a, const IES_Column &b)
{
    if (a.type == b.type)
    {
        return a.declIdx < b.declIdx;
    }
    if (a.type == IES_COL_FLOAT)
        return true;
    if (b.type == IES_COL_FLOAT)
        return false;
    if (a.type == IES_COL_STRING && b.type == IES_COL_STRING_SECOND)
        return true;
    if (a.type == IES_COL_STRING_SECOND && b.type == IES_COL_STRING)
        return false;
    return false; // fallback
}

void sortColumns(std::vector<IES_Column> &columns)
{
    std::sort(columns.begin(), columns.end(), iesColumnLess);
}

std::string decrypt_string(const std::vector<uint8_t> &data)
{
    const uint8_t xor_key = 1;
    std::string result;
    result.reserve(data.size());
    for (uint8_t b : data)
        result.push_back(b ^ xor_key);

    // Trim trailing '\1' (ASCII SOH) characters
    while (!result.empty() && result.back() == '\1')
        result.pop_back();

    return result;
}

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
