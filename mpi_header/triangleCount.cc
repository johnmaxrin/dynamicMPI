#include <stdio.h>
#include "mpi_graph.hpp"

int main(int argc, char *argv[])
{
    Graph graph(argv[1]);

    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;
    
    graph.initGraph(world);
    
    world.barrier();

    int triangleCount=0, globalCount=0;
    
    
    if(world.rank() == 0)
        printf("[0] TC Started\n");
     
 

    int start = world.rank() * graph.partitionSize;
    int end = start + graph.partitionSize - 1;


          

   
    // Triangle Count
    for (int v = start; v < end; v++)
    {
        for (int edge = graph.prefixSum[v]; edge < graph.prefixSum[v + 1]; edge++)
        {
            int u = graph.edgeList[edge].destination;
            if (u < v)
            {
                for (int edge = graph.prefixSum[v]; edge < graph.prefixSum[v + 1]; edge++)
                {
                    int w = graph.edgeList[edge].destination;
                    if (w > v)
                    {
                        if (graph.checkIfNeighbour(u, w))
                        {
                            triangleCount += 1;
                        }
                    }
                }
            }
        }
    }
   
    MPI_Reduce(&triangleCount,&globalCount,1,MPI_INT,MPI_SUM,0,world);
 
    if(world.rank()==0)
    printf("[%d] Triangle Count: %d\n",world.rank(), globalCount);
	
    return 0;
}
