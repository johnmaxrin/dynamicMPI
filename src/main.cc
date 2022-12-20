#include "../inc/mygraph.hpp"

int main(int c, char *argv[])
{

    Graph graph(argv[1]);
    
    MPI_Init(NULL, NULL);
    int rank, worldSize;
    GENMPI(&rank, &worldSize, COMMONWORLD);
    
    MPI_Datatype MPI_STAR_GRAPH;

    if (rank == 0)
    {

        int a = createMPIGraphDataType(&MPI_STAR_GRAPH);
        graph.initGraph();
        printf("%d Number of Nodes \n", graph.getNodeCount());
        MPI_Type_free(&MPI_STAR_GRAPH);
    }
    MPIBARRIER(COMMONWORLD);
    printf("Total Nodes: %d \n", graph.getNodeCount());
    graph.buildGraph(rank,worldSize);

    MPI_Finalize();
}