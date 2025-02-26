#include "application.h"

int main()
{
    Application app{};
    app.ipf_root.file_path = "/home/ridwan/Documents/TreeOfSaviorCN/patch/138241_001001.ipf";
    parse_ipf(app.ipf_root);
    if (!initialize(app))
        return -1;
    run(app);
    cleanup(app);
    return 0;
}