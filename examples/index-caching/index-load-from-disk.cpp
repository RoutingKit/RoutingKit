#include <routingkit/contraction_hierarchy.h>
#include <iostream>

using namespace RoutingKit;
using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << argv[0] << " << input.index >>\n";
        exit(-1);
    }

    // Load a precomputed graph
    auto ch = ContractionHierarchy::load_file(argv[1]);
}
