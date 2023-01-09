#include "../inc/mygraph.hpp"
#include "../global/mpiglobal.hpp"
#include <boost/serialization/serialization.hpp>

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

    ++totalNodes; // Graph starts from 0
    infile.close();
    printf("[#] Edges parsed \n");

    // Write code for sorting in MPI.
    // For this we need Total nodes, Total Process, divide the edges among process and do the sorting !
    // STARPLAT_MPI_SORT_NEIGHBOURS(edges)

    /* for(int i=0; i<totalNodes; ++i)
    {
        testEdge = edges[i];
        printf("Elements of %d \n",i);
        for(int j=0; j<testEdge.size(); ++j)
        {
            printf("%d ",testEdge[j].destination);
        }

        printf("\n");
    } */
}

// --- Paralell ---
void Graph::buildGraph(int rank, int size, int index)
{
    if (size != 0)
    {

        if (index % size == rank)
        {

            printf("Rank: %d Total Nodes: %d Total Edges: %d Edges Size: %d\n", rank, totalNodes, totalEdges, edges[0][0].destination);
        }
    }

    else
    {
        printf("[error] Number of processor cannot be zero");
    }
}
// END

int32_t Graph::getNodeCount() { return totalNodes; }
int32_t Graph::getEdgeCount() { return totalEdges; }
std::string Graph::getFileName() { return graphFile; }
std::map<int32_t, std::vector<Edge>> Graph::getEdges() { return edges; }

bool Graph::isEdge() { return true; }







