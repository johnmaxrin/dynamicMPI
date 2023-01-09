#include "../inc/mygraph.hpp"
#include <boost/mpi.hpp>

int main(int argc, char *argv[])
{
    Graph graph(argv[1]);

    
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;

    int rank = world.rank();

    if (rank == 0)
    {
        printf("[#] Inside rank 0\n");
        graph.initGraph();
    }

    MPIBARRIER(COMMONWORLD);
    boost::mpi::broadcast(world, graph, 0);
    MPIBARRIER(COMMONWORLD);

    for (int i = 0; i < graph.getNodeCount(); ++i)
        graph.buildGraph(rank, world.size(), i);

    MPI_Finalize();

    return 0;
}
