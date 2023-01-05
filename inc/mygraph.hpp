#include <fstream>
#include <sstream>
#include<iostream>
#include<vector>
#include<set>
#include<map>
#include<mpi.h>
#include<algorithm>
#include<string.h>
#include "../global/mpiglobal.hpp"

#ifndef MY_GRAPH
#define MY_GRAPH

class Edge{
    public:
        int32_t source;
        int32_t destination;
        int32_t weight;
        int32_t id;
};

class Graph{
    private:
        int32_t totalNodes;
        int32_t totalEdges;
        const char* graphFile;
        std::map<int32_t,std::vector<Edge>> edges;

    public:
    
    Graph(const char* file)
        {
        graphFile=file;
        totalNodes=0;
        totalEdges=0;
    }


    void initGraph();
    void buildGraph(int,int);

    int32_t getNodeCount();
    int32_t getEdgeCount();
    const char * getFileName();
    std::map<int32_t,std::vector<Edge>> getEdges(); 
    
    
    bool isEdge();
};

#endif