#include "application.h"

int main()
{
    std::cout << "Hello World\n";
    Application app{};
    parse_ipf("/home/ridwan/Documents/klaipeda/char_texture_low.ipf", app.ipf_root);
    if (!initialize(app))
        return -1;
    run(app);
    cleanup(app);
    return 0;
}