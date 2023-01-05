#include "../inc/mygraph.hpp"
#include "../utils/helpers/inc/mpiHelpers.hpp"

int main(int c, char *argv[])
{

    Graph graph(argv[1]);

    MPI_Init(NULL, NULL);
    int rank, worldSize;
    GENMPI(&rank, &worldSize, COMMONWORLD);

    MPI_Datatype MPI_STAR_GRAPH;

    int a = createMPIGraphDataType(&MPI_STAR_GRAPH, graph);

    if (rank == 0)
        printf("[#] Successfully created the Graph datatype \n");

    if (rank == 0)
        graph.initGraph();

    MPIBARRIER(COMMONWORLD);
    MPI_Bcast(&graph, 1, MPI_STAR_GRAPH,0,COMMONWORLD);
    MPIBARRIER(COMMONWORLD);
    graph.buildGraph(rank, worldSize);

    destroyMPIGraphDataType(&MPI_STAR_GRAPH);
    MPI_Finalize();
}
