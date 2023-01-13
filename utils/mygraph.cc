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

class MergeMapOp
{
public:
    std::map<int32_t, std::vector<Edge>> operator()(std::map<int32_t, std::vector<Edge>> &a, const std::map<int32_t, std::vector<Edge>> &b) const
    {
        for (const auto &[key, val] : b)
        {
            auto it = a.find(key);
            if (it != a.end())
            {
                it->second.insert(it->second.end(), val.begin(), val.end());
            }
            else
            {
                a.insert({key, val});
            }
        }

        return a;
    }

    MergeMapOp& operator=(const MergeMapOp &other) = default;

};

// --- @Paralell --- //
void Graph::initGraph(boost::mpi::communicator world)
{

    int32_t localTotalNodes = 0, localTotalEdges = 0;
    std::map<int32_t, vector<Edge>> localEdges, globalEdges;
    MPI_Offset readCount, offset;
    MPI_File file;
    boostFileIO(world, graphFile, file, readCount, offset, localTotalNodes, localTotalEdges, localEdges);
    MPI_File_close(&file);

    printf("[%d] Size: %ld\n", world.rank(), localEdges.size());


    if (world.rank() == 0)
    {

        boost::mpi::reduce(world, localEdges, globalEdges, MergeMapOp(), 0);
    }
    else
    {
        boost::mpi::reduce(world, localEdges, MergeMapOp(), 0);
    }

    if (world.rank() == 0)
    {
        printf("Global Size: %ld \n", globalEdges.size());
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
