#include "inc/graph.hpp"

int main(int argc, char *argv[])
{
    Graph graph(argv[1]);

    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;

    

    graph.parseGraph(world);


    // graph.buildGraph(world, graph);

    MPI_Finalize();

    return 0;
}
