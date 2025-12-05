#include "ipf/ipf_reader.hpp"
#include "ipf/utils.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <thread>

int main()
{

    std::string path = "C:\\Users\\Ridwan Hidayatullah\\Documents\\TreeOfSaviorCN\\data\\xml_tree.ipf";
    IPFRoot root;
    if (!readIpfRootFromPath(path, root))
    {
        for (auto &w : root.warnings)
            logWarn(w);
        logError("Failed reading IPF root");
        return 1;
    }
    logInfo("Header: file_count=" + std::to_string(root.header.file_count));
    for (auto &w : root.warnings)
        logWarn(w);
    logInfo("Listing first 20 file table entries:");
    for (size_t i = 0; i < root.file_table.size() && i < 20; ++i)
    {
        auto &e = root.file_table[i];
        std::cout << i << ": " << e.directory_name
                  << " compressed=" << e.file_size_compressed
                  << " uncompressed=" << e.file_size_uncompressed
                  << " pointer=" << e.file_pointer << "\n";
    }
    return 0;
}
