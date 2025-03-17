#include "application.h"

int main()
{
    Application app{};
    app.ipf_root.file_path = "C:\\Users\\Ridwan Hidayatullah\\Documents\\TreeOfSaviorCN\\data\\ui.ipf";
    parse_ipf(app.ipf_root);
    if (!initialize(app))
        return -1;
    run(app);
    cleanup(app);
    return 0;
}