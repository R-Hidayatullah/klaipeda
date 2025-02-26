#include "application.h"

int main()
{
    std::cout << "Hello World\n";
    Application app{};
    parse_ipf("/home/ridwan/Documents/klaipeda/char_texture_low.ipf", app.ipf_root);
    print_debug_info(app.ipf_root);
    std::vector<uint8_t> result_data = extract_data("/home/ridwan/Documents/klaipeda/char_texture_low.ipf", app.ipf_root, 0);
    print_hex_dump(result_data, 16, 4);
    std::vector<uint8_t> decompressed_data = decompress_data(result_data, app.ipf_root.ipf_file_table[0].file_size_uncompressed);
    print_hex_dump(decompressed_data, 16, 4);

    // if (!initialize(app))
    //     return -1;
    // run(app);
    // cleanup(app);
    return 0;
}