#include "mpi_graph.hpp"

int main(int argc, char * argv[])
{
    Graph g(argv[1]);
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;

    g.initGraph(world);

    printf("[%d] Done. \n",world.rank());

    return 0;
}