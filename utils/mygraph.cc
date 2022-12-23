#include"../inc/mygraph.hpp"
#include"../global/mpiglobal.hpp"

using namespace std;


void Graph::initGraph()
{
    std::fstream infile;
    
    std::string line;
    infile.open(graphFile);

    
    if(!infile.is_open()){
        printf("[error] File not found \n");
        exit(0);
    }

    printf("[#] Building graph from %s\n",graphFile);
    while(std::getline(infile,line)){
        
        if(line.length()==0 || line[0] < '0' || line[0] > '9')
            continue;
        
        std::stringstream ss(line);
        totalEdges++;

        Edge edge;
        int32_t source;
        int32_t destination;
        int32_t weight;

        ss>>source;
        if(source > totalNodes)
            totalNodes = source;
        
        ss>>destination;
        if(destination > totalNodes)
            totalNodes = destination;

        edge.source = source;
        edge.destination = destination;
        edge.weight = 1;
        edge.id = source;

        edges[source].push_back(edge);

        ss>>weight;
        
    }

    ++totalNodes; // Graph starts from 0
    infile.close();
    printf("[#] Edges parsed \n");

    printf("[#] Showing edges \n");
    std::vector<Edge> testEdge;

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

    printf("[#] Graph built successfully\n");

}

// --- Paralell --- 
void Graph::buildGraph(int rank, int size)
{   
    if(size != 0){
    
    int n = totalNodes/size;
    printf("%d --> in Rank %d \n",n,rank);
    }

    else{
        printf("[error] Number of processor cannot be zero");
    }
    
}
// END


int32_t Graph::getNodeCount() {return totalNodes;}

//std::map<int32_t,std::vector<Edge>> Graph::getEdges() {return edges;}
bool Graph::isEdge() {return true;}
