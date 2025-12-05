#include "ipf/decompress.hpp"

#include <zlib.h>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

bool decompressZlib(const std::vector<uint8_t> &in, std::vector<uint8_t> &out, std::string &err)
{
    if (in.empty())
    {
        out.clear();
        return true;
    }

    z_stream zs;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;
    zs.next_in = const_cast<Bytef *>(in.data());
    zs.avail_in = static_cast<uInt>(in.size());

    // -MAX_WBITS to indicate raw deflate (no zlib header). Use MAX_WBITS for zlib header.
    int ret = inflateInit2(&zs, -MAX_WBITS);
    if (ret != Z_OK)
    {
        err = "inflateInit2 failed";
        return false;
    }

    std::vector<uint8_t> buffer;
    size_t out_capacity = std::max<size_t>(in.size() * 3, 1024);
    buffer.resize(out_capacity);

    zs.next_out = buffer.data();
    zs.avail_out = static_cast<uInt>(buffer.size());

    while (true)
    {
        ret = inflate(&zs, Z_NO_FLUSH);
        if (ret == Z_STREAM_END)
            break;
        if (ret != Z_OK)
        {
            inflateEnd(&zs);
            err = "inflate failed with code " + std::to_string(ret);
            return false;
        }
        // need more space
        size_t used = buffer.size() - zs.avail_out;
        buffer.resize(buffer.size() * 2);
        zs.next_out = buffer.data() + used;
        zs.avail_out = static_cast<uInt>(buffer.size() - used);
    }

    size_t decompressed = buffer.size() - zs.avail_out;
    out.assign(buffer.begin(), buffer.begin() + decompressed);

    inflateEnd(&zs);
    return true;
}
