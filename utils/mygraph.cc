/*
Starplat MPI Graph class implementation.

--- Provide documentation here ---

@Starplat MPI backend.
*/

#include "../inc/mygraph.hpp"

using namespace std;

void merge_maps(std::map<int, std::vector<Edge>> &a, std::map<int, std::vector<Edge>> &b)
{
    for (auto &[key, value] : b)
    {
        a[key].insert(a[key].end(), value.begin(), value.end());
    }
}



// --- @Paralell --- //
void Graph::initGraph(boost::mpi::communicator world)
{

    int32_t localTotalNodes = 0, localTotalEdges = 0;
    std::map<int32_t, vector<Edge>> localEdges;
    MPI_Offset readCount, offset;
    MPI_File file;
    printf("Hi\n");
    boostFileIO(world, graphFile, file, readCount, offset, localTotalNodes, localTotalEdges, localEdges);
    MPI_File_close(&file);

    printf("[%d] Size: %ld\n", world.rank(), localEdges.size());

    int num_nodes ;  
    num_nodes = (int)world.rank();
    for(auto it = localEdges.begin();it!=localEdges.end();it++)
    {
        num_nodes = max(num_nodes, (*it).first);
    }

    num_nodes = boost::mpi::all_reduce(world, num_nodes, boost::mpi::maximum<int>())+1;
    cout<<"num "<<num_nodes<<endl;

    int vertex_partition_size = (num_nodes+world.size()-1)/world.size();

    std::vector<std::vector<Edge>> adjacency_list_group(vertex_partition_size);

    std::vector<std::vector<std::vector<Edge>>> adjacency_list_3d(world.size(),adjacency_list_group);
    
    std::vector<int> len(num_nodes);
    for(auto it = localEdges.begin();it!=localEdges.end();it++)
    {
        int source = (*it).first;
        int proc_num = source/vertex_partition_size;
        adjacency_list_3d[proc_num][source % vertex_partition_size].insert(adjacency_list_3d[proc_num][source % vertex_partition_size].end(),(*it).second.begin(),(*it).second.end());
        len[source] = (*it).second.size();
    }
    int * index_of_nodes = new int [num_nodes];
    MPI_Allreduce(len.data(),index_of_nodes,len.size(),MPI_INT,MPI_SUM,MPI_COMM_WORLD);
    cout<<"reduce_done"<<endl;
    boost::mpi::all_to_all(world, adjacency_list_3d, adjacency_list_3d);
    cout<<"all done"<<endl;
    
    std::vector<Edge> local_csr;
    for(int i=0;i<vertex_partition_size;i++)
    {   
        for(int j=0;j<world.size();j++)
        {
            local_csr.insert(local_csr.end(), adjacency_list_3d[j][i].begin(),adjacency_list_3d[j][i].end());
        }
    }

    if(world.rank()==0)
    {
        for(int i=0;i<num_nodes;i++)
        {
            cout<<index_of_nodes[i]<<" ";
        }cout<<endl;
        for(int i=0;i<local_csr.size();i++)
        {
            cout<<local_csr[i].destination<<" ";
        }cout<<endl;

    }
}

// ---  @Serial  --- //
void Graph::buildGraph(boost::mpi::communicator world, Graph &graph)
{
    if (world.size() != 0)
    {

        /* Sort Edge List */

        /*
            Create edge vector in order to use scatter.
            Our edges are store as map but Boost scatter doesn't
            have the ability to scatter Map among process. So we
            will be converting it to a vector and after scattering
            converting back to map.
        */
        std::vector<std::pair<int, std::vector<Edge>>> edgesVector;
        std::vector<int> recvCount(world.size());

        if (world.rank() == MASTER)
            boostConvertEdgestoVector(edgesVector, graph);

        boost::mpi::broadcast(world, edgesVector, MASTER);

        boostPopulateDisplacement(totalNodes, world.size(), recvCount);
        std::vector<pair<int32_t, std::vector<Edge>>> localEdgesVector(recvCount[world.rank()]);

        // To be removed for testing only.

        boost::mpi::scatterv(world, edgesVector.data(), recvCount, localEdgesVector.data(), MASTER);
        std::map<int32_t, std::vector<Edge>> localEdgesMap;
        boostConvertVectorstoEdges(localEdgesMap, localEdgesVector);

        // Do the sorting -- Paralell --
        for (int32_t i = 0; i < localEdgesMap.size(); ++i)
            boostSortNeighbours(localEdgesMap[i]);

        if (world.rank() == 0)
            printf("[#] Done sorting \n");

        // for(int32_t i=0; i<localEdgesMap.size(); ++i)
        // {
        //     std::vector<Edge> &e = localEdgesMap[i];
        //     for(Edge edge: e)
        //         printf("%d ",edge.destination);

        //     printf("~");
        // }
    }

    else
    {
        printf("[error] Number of processor cannot be zero");
        exit(0);
    }
}
// END

int32_t Graph::getNodeCount() { return totalNodes; }
int32_t Graph::getEdgeCount() { return totalEdges; }
std::string Graph::getFileName() { return graphFile; }
std::map<int32_t, std::vector<Edge>> Graph::getEdges() { return edges; }

bool Graph::isEdge() { return true; }
