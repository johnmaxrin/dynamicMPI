#ifndef MPI_GRAPH_HEADER
#define MPI_GRAPH_HEADER

/** Header Files **/
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <mpi.h>
#include <string.h>
#include <algorithm>

/** Boost and MPI Specific **/
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>

class Edge
{
public:
    int32_t source;
    int32_t destination;
    int32_t weight;
    int32_t id;

private:
    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar &source;
        ar &destination;
        ar &weight;
        ar &id;
    }
};

class Graph
{
private:
    int32_t totalNodes;
    int32_t totalEdges;
    std::string graphFile;
    std::map<int32_t, std::vector<Edge>> edges;

    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar &totalNodes;
        ar &totalEdges;
        ar &graphFile;
        ar &edges;
    }

public:
    /*** Data Members ***/

    int32_t *indexofNodes;        /* Outneighbours of a node*/
    int32_t *rev_indexofNodes;    /* In neighbours of a node*/
    std::vector<Edge> edgeList;   /* Edge List */
    std::vector<Edge> sourceList; /* Source List */
    int32_t partitionSize;        /* For loops */
    std::vector<int32_t> prefixSum;
    /*** Member Functions ***/

    Graph(std::string file)
    {
        graphFile = file;
        totalNodes = 0;
        totalEdges = 0;
    }

    void initGraph(boost::mpi::communicator world)
    {

        int32_t num_nodes=0, num_edges=0;
        int scatter_size;
        std::vector<Edge> Edges;
        if (world.rank() == 0)
        {
            readFromFile(graphFile, num_nodes, num_edges, Edges);
            num_nodes = num_nodes + 1;
	    printf("[0] Reading completed. Number of edges %d\n",num_edges);
            assert(num_edges == Edges.size());
        }

        world.barrier();

        boost::mpi::broadcast(world, num_nodes, 0);
        boost::mpi::broadcast(world, num_edges, 0);
        totalNodes = num_nodes;
        totalEdges = num_edges;

        std::vector<int> sizes;
        for (int i = 0; i < world.size(); i++)
            sizes.push_back(num_edges / world.size() + (i < (num_edges % world.size()) ? 1 : 0));

        scatter_size = num_edges / world.size() + (world.rank() < (num_edges % world.size()) ? 1 : 0);
		
	//printf("[%d] Scatter begins. Size: %d \n",world.rank(),scatter_size);	
        Edge *localEdges = new Edge[scatter_size];
        boost::mpi::scatterv(world, Edges, sizes, localEdges, 0);

	//printf("[%d] Scatter completed. \n",world.rank());

       int vertex_partition_size = (num_nodes + world.size() - 1) / world.size();
        partitionSize = vertex_partition_size;

        std::vector<std::vector<Edge>> adjacency_list_group(vertex_partition_size);

        std::vector<std::vector<std::vector<Edge>>> adjacency_list_3d(world.size(), adjacency_list_group);
        std::vector<std::vector<std::vector<Edge>>> rev_adjacency_list_3d(world.size(), adjacency_list_group);

        std::vector<int> index(num_nodes, 0);
        std::vector<int> rev_index(num_nodes, 0);
        for (int i = 0; i < scatter_size; i++)
        {
            int source = localEdges[i].source;
            int destination = localEdges[i].destination;
            int proc_num = source / vertex_partition_size;
            adjacency_list_3d[proc_num][source % vertex_partition_size].push_back((localEdges[i]));
            index[source] = index[source] + 1;

            proc_num = destination / vertex_partition_size;
            rev_adjacency_list_3d[proc_num][destination % vertex_partition_size].push_back((localEdges[i]));
            rev_index[destination] = rev_index[destination] + 1;
        }
        delete[] localEdges;
        indexofNodes = new int[num_nodes];
        rev_indexofNodes = new int[num_nodes];
	//printf("[%d] All Reduce started. Index Size: %ld  \n",world.rank(),index.size()); 
        MPI_Allreduce(index.data(), indexofNodes, index.size(), MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        index.clear();

	//printf("[%d] One all reduce done \n",world.rank());

        MPI_Allreduce(rev_index.data(), rev_indexofNodes, rev_index.size(), MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        rev_index.clear();

	//printf("[%d] All Reduce done.\n",world.rank());

        boost::mpi::all_to_all(world, adjacency_list_3d, adjacency_list_3d);
        boost::mpi::all_to_all(world, rev_adjacency_list_3d, rev_adjacency_list_3d);
	
	//printf("[%d] All to all done. \n",world.rank());	

        for (int i = 0; i < vertex_partition_size; i++)
        {
            std::vector<Edge> temp1, temp2;
            for (int j = 0; j < world.size(); j++)
            {
                temp1.insert(temp1.end(), adjacency_list_3d[j][i].begin(), adjacency_list_3d[j][i].end());
                temp2.insert(temp2.end(), rev_adjacency_list_3d[j][i].begin(), rev_adjacency_list_3d[j][i].end());
            }

            sort(temp1.begin(), temp1.end(), comp_destination);
            sort(temp2.begin(), temp2.end(), comp_source);

            edgeList.insert(edgeList.end(), temp1.begin(), temp1.end());
            sourceList.insert(sourceList.end(), temp2.begin(), temp2.end());
            
	}

	//printf("[%d] All done. \n",world.rank());
	if(world.rank() == 0)
          createPrefixSum();
	world.barrier();
        boost::mpi::broadcast(world, prefixSum,0);
    }

    bool isEdge();

    /** Helper Functions **/
    static bool comp_source(Edge &a, Edge &b) { return a.source < b.source; }
    static bool comp_destination(Edge &a, Edge &b) { return a.destination < b.destination; }
    int32_t getTotalNodes() { return totalNodes; }

    bool checkIfNeighbour(int32_t source, int32_t destination)
    {
        for (Edge e : getNeighbours(source))
        {
            int nbr = e.destination;
            if (nbr == destination)
                return true;
        }
        return false;
    }

    void createPrefixSum()
    {
	 
        prefixSum.resize(totalNodes+1);
        prefixSum[0] = 0;
        for (int i = 1; i < totalNodes+1; i++)
            prefixSum[i] = prefixSum[i - 1] + indexofNodes[i];
    }

    std::vector<Edge> getNeighbours(int32_t source)
    {	
        std::vector<Edge> outNeighbours;
        for (int i = prefixSum[source]; i < prefixSum[source + 1]; i++)
        {
            int nbr = edgeList[i].destination;
            if (nbr != INT_MAX / 2) // Remove this condition.
            {
                Edge e;
                e.source = source;
                e.destination = nbr;
                e.weight = 1; // Haven't handles weights now.
                e.id = i;
                outNeighbours.push_back(e);
            }
        }
        return outNeighbours;

        // -- Update part --

        // if (diff_edgeList != NULL)
        // {
        //     for (int j = diff_indexofNodes[node]; j < diff_indexofNodes[node + 1]; j++)
        //     {
        //         int nbr = diff_edgeList[j];
        //         if (nbr != INT_MAX / 2)
        //         {
        //             edge e;
        //             e.source = node;
        //             e.destination = nbr;
        //             e.weight = diff_edgeLen[j];
        //             e.id = edgesTotal + j;
        //             // printf(" weight %d\n", e.weight);
        //             out_edges.push_back(e);
        //         }
        //     }
        // }
    }

    void readFromFile(std::string filePath, int32_t &nodes, int32_t &edges, std::vector<Edge> &localedges)
    {
        nodes = 0;
        edges = 0;

        std::ifstream infile;
        infile.open(filePath);
        std::string line;

        while (std::getline(infile, line))
        {

            if (line.length() == 0 || line[0] < '0' || line[0] > '9')
            {
                continue;
            }

            std::stringstream ss(line);

            edges++;

            Edge e;

            int32_t source;
            int32_t destination;
            int32_t weightVal;

            ss >> source;
            if (source > nodes)
                nodes = source;

            ss >> destination;
            if (destination > nodes)
                nodes = destination;

            e.source = source;
            e.destination = destination;
            e.weight = 1;
            e.id = source;

            ss >> weightVal;

            localedges.push_back(e);
        }

        infile.close();
    }
};

#endif
