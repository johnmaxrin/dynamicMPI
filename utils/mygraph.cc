/*
Starplat MPI Graph class implementation.

--- Provide documentation here ---

@Starplat MPI backend.
*/

#include "../inc/mygraph.hpp"

using namespace std;

void Graph::initGraph()
{
    std::fstream infile;

    std::string line;
    infile.open(graphFile);

    if (!infile.is_open())
    {
        printf("[error] File not found \n");
        exit(0);
    }

    printf("[#] Building graph from %s\n", graphFile.c_str());
    while (std::getline(infile, line))
    {

        if (line.length() == 0 || line[0] < '0' || line[0] > '9')
            continue;

        std::stringstream ss(line);
        totalEdges++;

        Edge edge;
        int32_t source;
        int32_t destination;
        int32_t weight;

        ss >> source;
        if (source > totalNodes)
            totalNodes = source;

        ss >> destination;
        if (destination > totalNodes)
            totalNodes = destination;

        edge.source = source;
        edge.destination = destination;
        edge.weight = 1;
        edge.id = source;

        edges[source].push_back(edge);

        ss >> weight;
    }

    ++totalNodes;

    infile.close();
    printf("[#] Edges parsed \n");

    /*
        Our graphs are designed so that the
        node with the largest number +1
        gives us the total number of nodes.
    */
}

// --- Paralell --- //
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

        if(world.rank() == MASTER)
            convertEdgestoVector(edgesVector,graph);
        
        boost::mpi::broadcast(world,edgesVector,MASTER);
        
        for(int i=0; i<world.size(); ++i)
            recvCount[i] = (totalNodes/world.size()) + (i<(totalNodes%world.size()));

        
        std::vector<pair<int32_t ,std::vector<Edge>>> localEdgesVector(recvCount[world.rank()]);
        
        boost::mpi::scatterv(world,edgesVector.data(),recvCount,localEdgesVector.data(),MASTER);
        std::map<int32_t, std::vector<Edge>> localEdgesMap;
        convertVectorstoEdges(localEdgesMap,localEdgesVector);

        printf("Size: %ld",localEdgesMap.size());


        // edgesVector is now a vector of edges.

       // for (const auto &[key, value] : edged)
       // {
       //     edgesVector.emplace_back(key, value);
       // }

        //boost::mpi::scatter(world, edges, localEdges, 0);
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
