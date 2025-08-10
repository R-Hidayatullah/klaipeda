#if !defined(IES_H)
#define IES_H
#include "klaipeda.h"

constexpr size_t DEFAULT_CHAR_LEN = 64;

enum IES_ColumnType : uint16_t
{
    IES_COL_FLOAT = 0,
    IES_COL_STRING = 1,
    IES_COL_STRING_SECOND = 2,
};

struct IES_Header
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

typedef struct
{
    std::string column; // length: 64 chars
    std::string name;   // length: 64 chars
    IES_ColumnType type;
    uint16_t access;
    uint16_t sync;
    uint16_t declIdx;
} IES_Column;

typedef struct
{
    bool has_float = false;
    float value_float = 0.0f;

    bool has_int = false;
    uint32_t value_int = 0;

    bool has_string = false;
    std::string value_string;
} IES_Row;

typedef struct
{
    IES_Header ies_header;
    std::vector<IES_Column> ies_column;
    std::vector<std::vector<IES_Row>> ies_row;
    std::string file_path;
} IES_Root;

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

// Parsing function for IES
bool parse_ies(IES_Root &ies_root)
{
    std::ifstream file(ies_root.file_path, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open file: " << ies_root.file_path << '\n';
        return false;
    }

    // --- Parse Header ---

    // idspace and keyspace are 64 chars each
    read(file, ies_root.ies_header.idspace, 64);
    read(file, ies_root.ies_header.keyspace, 64);

    // Other header numeric fields
    read(file, ies_root.ies_header.version);
    read(file, ies_root.ies_header.padding);
    read(file, ies_root.ies_header.infoSize);
    read(file, ies_root.ies_header.dataSize);
    read(file, ies_root.ies_header.totalSize);
    read(file, ies_root.ies_header.useClassID);
    read(file, ies_root.ies_header.padding2);
    read(file, ies_root.ies_header.numField);
    read(file, ies_root.ies_header.numColumn);
    read(file, ies_root.ies_header.numColumnNumber);
    read(file, ies_root.ies_header.numColumnString);
    read(file, ies_root.ies_header.padding3);

    // --- Parse Columns ---
    std::streamoff column_offset = -static_cast<std::streamoff>(ies_root.ies_header.infoSize + ies_root.ies_header.dataSize);

    // Seek from end by offset
    file.seekg(column_offset, std::ios_base::end);

    // Check if seek succeeded
    if (!file)
    {
        std::cerr << "Failed to seek to position\n";
        // handle error
    }

    uint16_t col_count = ies_root.ies_header.numColumn;
    ies_root.ies_column.resize(col_count);

    for (uint16_t i = 0; i < col_count; ++i)
    {
        // Temporary buffer to read 64 bytes
        std::vector<uint8_t> column(64);

        read(file, column, 64);

        // Decrypt and assign to column name
        ies_root.ies_column[i].column = decrypt_string(column);

        std::vector<uint8_t> name(64);
        read(file, name, 64);

        // Decrypt and assign to column name
        ies_root.ies_column[i].name = decrypt_string(name);

        // Read other fields
        // Read uint16_t from file into a temporary variable
        uint16_t raw_type;
        read(file, raw_type); // your existing read function reading a uint16_t

        // Convert raw_type to enum safely
        switch (raw_type)
        {
        case 0:
            ies_root.ies_column[i].type = IES_COL_FLOAT;
            break;
        case 1:
            ies_root.ies_column[i].type = IES_COL_STRING;
            break;
        case 2:
            ies_root.ies_column[i].type = IES_COL_STRING_SECOND;
            break;
        default:
            throw std::runtime_error("Invalid column type");
        }

        read(file, ies_root.ies_column[i].access);
        read(file, ies_root.ies_column[i].sync);
        read(file, ies_root.ies_column[i].declIdx);

        // Optional: trim trailing nulls in strings (already done by read for std::string)
    }

    // --- Parse Rows ---

    std::streamoff row_offset = -static_cast<std::streamoff>(ies_root.ies_header.dataSize);

    // Seek from end by offset
    file.seekg(row_offset, std::ios_base::end);

    // Check if seek succeeded
    if (!file)
    {
        std::cerr << "Failed to seek to position\n";
        // handle error
    }

    ies_root.ies_row.clear();
    const uint16_t row_count = ies_root.ies_header.numField; // or row_count if you have it
    sortColumns(ies_root.ies_column);

    for (uint16_t row_idx = 0; row_idx < row_count; ++row_idx)
    {
        // Read 4-byte padding (discard)
        uint32_t padding = 0;
        read(file, padding);

        // Read count of bytes for the row (uint16_t)
        uint16_t count = 0;
        read(file, count);

        // Read 'count' bytes buffer (you might or might not need this depending on format)
        std::vector<uint8_t> buffer(count);
        file.read(reinterpret_cast<char *>(buffer.data()), count);
        if (!file)
        {
            std::cerr << "Failed reading row buffer\n";
            return false;
        }

        // Prepare vector for one row's values
        std::vector<IES_Row> row;
        row.reserve(ies_root.ies_column.size());

        // For each column in this row
        for (const IES_Column &column : ies_root.ies_column)
        {
            IES_Row value = {}; // zero-initialize

            if (column.type == IES_COL_FLOAT)
            {
                float fvalue = 0.0f;
                read(file, fvalue);

                union
                {
                    uint32_t i;
                    float f;
                } converter;
                converter.i = 0xFFFFFFFF;

                float max_value = converter.f;

                if (std::fabs(fvalue - max_value) < std::numeric_limits<float>::epsilon())
                {
                    value.value_float = max_value;
                    value.has_float = true;
                }
                else
                {
                    value.value_int = static_cast<uint32_t>(fvalue);
                    value.has_int = true;
                }
            }
            else
            {
                uint16_t strlen = 0;
                read(file, strlen);

                std::vector<uint8_t> strbuf(strlen);
                file.read(reinterpret_cast<char *>(strbuf.data()), strlen);
                if (!file)
                {
                    std::cerr << "Failed reading string in row\n";
                    return false;
                }

                std::string s = decrypt_string(strbuf);

                if (!s.empty())
                {
                    value.value_string = s;
                    value.has_string = true;
                }
            }

            row.push_back(std::move(value));
        }

        ies_root.ies_row.push_back(std::move(row));

        file.seekg(ies_root.ies_header.numColumnString, std::ios_base::cur);
        if (!file)
        {
            std::cerr << "Failed to seek after row\n";
            return false;
        }
    }
    return true;
}

void print_ies_debug(const IES_Root &ies_root)
{
    // Print Header info
    const IES_Header &h = ies_root.ies_header;
    std::cout << "=== IES Header ===\n";
    std::cout << "idspace: " << h.idspace << "\n";
    std::cout << "keyspace: " << h.keyspace << "\n";
    std::cout << "version: " << h.version << "\n";
    std::cout << "infoSize: " << h.infoSize << "\n";
    std::cout << "dataSize: " << h.dataSize << "\n";
    std::cout << "totalSize: " << h.totalSize << "\n";
    std::cout << "useClassID: " << static_cast<int>(h.useClassID) << "\n";
    std::cout << "numField: " << h.numField << "\n";
    std::cout << "numColumn: " << h.numColumn << "\n";
    std::cout << "numColumnNumber: " << h.numColumnNumber << "\n";
    std::cout << "numColumnString: " << h.numColumnString << "\n\n";

    // Print Columns info
    std::cout << "=== IES Columns ===\n";
    for (size_t i = 0; i < ies_root.ies_column.size(); ++i)
    {
        const IES_Column &c = ies_root.ies_column[i];
        std::cout << "Column " << i << ":\n";
        std::cout << "  column: " << c.column << "\n";
        std::cout << "  name: " << c.name << "\n";
        std::cout << "  type: ";
        switch (c.type)
        {
        case IES_COL_FLOAT:
            std::cout << "FLOAT";
            break;
        case IES_COL_STRING:
            std::cout << "STRING";
            break;
        case IES_COL_STRING_SECOND:
            std::cout << "STRING_SECOND";
            break;
        default:
            std::cout << "UNKNOWN";
            break;
        }
        std::cout << "\n";
        std::cout << "  access: " << c.access << "\n";
        std::cout << "  sync: " << c.sync << "\n";
        std::cout << "  declIdx: " << c.declIdx << "\n\n";
    }

    // Print Rows info
    std::cout << "=== IES Rows ===\n";
    for (size_t i = 0; i < ies_root.ies_row.size(); ++i)
    {
        std::cout << "Row " << i << ":\n";
        const auto &row = ies_root.ies_row[i];
        for (size_t j = 0; j < row.size(); ++j)
        {
            const IES_Row &r = row[j];
            std::cout << "  Col " << j << ": ";
            if (r.has_string)
                std::cout << "String = " << r.value_string;
            else if (r.has_float)
                std::cout << "Float = " << r.value_float;
            else if (r.has_int)
                std::cout << "Int = " << r.value_int;
            else
                std::cout << "No value";
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}

#endif // IES_H
