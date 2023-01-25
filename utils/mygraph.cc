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


bool comp_source(Edge & a, Edge & b)
{
    return a.source < b.source;
}
bool comp_destination(Edge & a, Edge & b)
{
    return a.destination < b.destination;
}

// --- @Paralell --- //
void Graph::initGraph(boost::mpi::communicator world)
{

    int32_t localTotalNodes = 0, localTotalEdges = 0;
    

    //MPI_Offset readCount, offset;
    //MPI_File file;
    //boostFileIO(world, graphFile, file, readCount, offset, localTotalNodes, localTotalEdges, localEdges);
    //MPI_File_close(&file);


    int num_nodes, num_edges;
    int scatter_size;
    std::vector<Edge> Edges;
    if(world.rank()==0)
    {   
        printf("Started reading from file\n");
        readFromFile(graphFile, num_nodes, num_edges, Edges);
        num_nodes = num_nodes +1;
        printf("Finished reading from file\n");
        assert(num_edges == Edges.size());
    }
    
    MPIBARRIER(COMMONWORLD);
    boost::mpi::broadcast(world, num_edges,0);
    vector<int> sizes;
    for(int i=0;i<world.size();i++)
    {
        sizes.push_back(num_edges/world.size() +  (i < (num_edges % world.size()) ? 1 : 0));
    }
    scatter_size = num_edges/world.size() +  (world.rank() < (num_edges % world.size()) ? 1 : 0) ;
    boost::mpi::broadcast(world, num_nodes,0);
    //cout<<num_edges<<" "<<num_nodes<<" "<<scatter_size<<endl;

    Edge * localEdges = new Edge [scatter_size]; 
    if(world.rank() ==0)printf("Started Scatter\n");
    boost::mpi::scatterv(world, Edges, sizes, localEdges, 0);
    if(world.rank() ==0)printf("Finished Scatter\n");
    
    
    //num_nodes = (int)world.rank();
    //for (auto it = localEdges.begin(); it != localEdges.end(); it++)
    //{
    //    num_nodes = max(num_nodes, (*it).first);
   // }

    //num_nodes = boost::mpi::all_reduce(world, num_nodes, boost::mpi::maximum<int>()) + 1;
    //cout << "num " << num_nodes << endl;

    
    std::vector<Edge> local_csr;
    std::vector<Edge> rev_local_csr;
    
    int vertex_partition_size = (num_nodes + world.size() - 1) / world.size();

    std::vector<std::vector<Edge>> adjacency_list_group(vertex_partition_size);
    
    std::vector<std::vector<std::vector<Edge>>> adjacency_list_3d(world.size(), adjacency_list_group);
    std::vector<std::vector<std::vector<Edge>>> rev_adjacency_list_3d(world.size(), adjacency_list_group);

    std::vector<int> index(num_nodes,0);
    std::vector<int> rev_index(num_nodes,0);
    for (int i = 0; i < scatter_size; i++)
    {
        int source = localEdges[i].source;
        int destination = localEdges[i].destination;
        //cout<<world.rank()<<" "<<source<<" "<<destination<<" hiii "<<endl;
        int proc_num = source / vertex_partition_size;
        adjacency_list_3d[proc_num][source % vertex_partition_size].push_back((localEdges[i]));
        index[source] = index[source]+ 1;

        //if(destination >= num_nodes)
        //    {cout<<"should not print"<<endl; exit(-1);}

        proc_num = destination / vertex_partition_size;
        rev_adjacency_list_3d[proc_num][destination % vertex_partition_size].push_back((localEdges[i]));
         rev_index[destination] = rev_index[destination] +1;
    }
    delete [] localEdges;
    int *index_of_nodes = new int[num_nodes];
    int *rev_index_of_nodes = new int[num_nodes];
    if(world.rank() ==0)printf("Started Reduce\n");
    MPI_Allreduce(index.data(), index_of_nodes, index.size(), MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    index.clear();
    
    MPI_Allreduce(rev_index.data(), rev_index_of_nodes, rev_index.size(), MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    rev_index.clear();
    
    if(world.rank() ==0)printf("Finished Reduce\n");
    if(world.rank() ==0)printf("Started All to All\n");
    if(world.rank() ==31)printf("Started All to All\n");
    boost::mpi::all_to_all(world, adjacency_list_3d, adjacency_list_3d);
    boost::mpi::all_to_all(world, rev_adjacency_list_3d, rev_adjacency_list_3d);
    if(world.rank() ==0)printf("Finished all to all\n");
    if(world.rank() ==31)printf("Finished All to All\n");

    
    for (int i = 0; i < vertex_partition_size; i++)
    {
        vector<Edge> temp1, temp2;
        for (int j = 0; j < world.size(); j++)
        {
            temp1.insert(temp1.end(), adjacency_list_3d[j][i].begin(), adjacency_list_3d[j][i].end());
            temp2.insert(temp2.end(), rev_adjacency_list_3d[j][i].begin(), rev_adjacency_list_3d[j][i].end());
        }

        sort(temp1.begin(),temp1.end(), comp_destination);
        sort(temp2.begin(),temp2.end(), comp_source);

        local_csr.insert(local_csr.end(), temp1.begin(), temp1.end());
        rev_local_csr.insert(rev_local_csr.end(), temp2.begin(), temp2.end());
    }
    
    /*if (world.rank() == 0)
    {
        for (int i = 0; i < num_nodes; i++)
        {
            cout << index_of_nodes[i] << " ";
        }
        cout << endl;
        for (int i = 0; i < local_csr.size(); i++)
        {
            cout << local_csr[i].destination << " ";
        }
        cout << endl;

        for (int i = 0; i < num_nodes; i++)
        {
            cout << rev_index_of_nodes[i] << " ";
        }
        cout << endl;
        for (int i = 0; i < rev_local_csr.size(); i++)
        {
            cout << rev_local_csr[i].source << " ";
        }
        cout << endl;
    }*/ 
}



// ---  @Serial  --- //
void Graph::buildGraph(boost::mpi::communicator world, Graph &graph)
{
    if (world.size() != 0)
    {

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
