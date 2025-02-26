#include "application.h"

int main()
{
    std::cout << "Hello World\n";
    Application app{};
    app.ipf_root.file_path = "/home/ridwan/Documents/TreeOfSaviorCN/data/ui.ipf";
    parse_ipf(app.ipf_root);
    print_debug_info(app.ipf_root);
    std::vector<uint8_t> result_data = extract_data(app.ipf_root, 0);
    print_hex_dump(result_data, 16, 4);

    if (!initialize(app))
        return -1;
    run(app);
    cleanup(app);
    return 0;
}