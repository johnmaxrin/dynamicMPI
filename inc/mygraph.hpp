#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <mpi.h>
#include <algorithm>
#include <string.h>

#include "../global/mpiglobal.hpp"
#include "../utils/helpers/inc/boostMPIHelpers.hpp"
#include "../utils/helpers/inc/graphHelper.hpp"


#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/serialization.hpp>


#ifndef MY_GRAPH
#define MY_GRAPH

class Edge
{
public:
    int32_t source;
    int32_t destination;
    int32_t weight;
    int32_t id;

private:
    // Boost Serialization
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
    
    // F.R.I.E.N.D.S
    friend void convertEdgestoVector(std::vector<std::pair<int, std::vector<Edge>>> &edgesVector, Graph &graph);
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
    Graph(std::string file)
    {
        graphFile = file;
        totalNodes = 0;
        totalEdges = 0;
    }

    void initGraph();
    void buildGraph(boost::mpi::communicator, Graph &graph);

    int32_t getNodeCount();
    int32_t getEdgeCount();
    std::string getFileName();
    std::map<int32_t, std::vector<Edge>> getEdges();

    bool isEdge();
};

#endif