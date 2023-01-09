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
#include <boost/mpi.hpp>
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
        std::string  graphFile;
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
        Graph(std::string file)
        {
            graphFile = file;
            totalNodes = 0;
            totalEdges = 0;
        }

        void initGraph();
        void buildGraph(int, int, int);

        int32_t getNodeCount();
        int32_t getEdgeCount();
        std::string getFileName();
        std::map<int32_t, std::vector<Edge>> getEdges();

        bool isEdge();
    };

#endif