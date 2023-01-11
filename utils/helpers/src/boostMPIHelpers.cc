#include "../inc/boostMPIHelpers.hpp"


using namespace std;

void convertEdgestoVector(std::vector<std::pair<int, std::vector<Edge>>> &edgesVector, Graph &graph)
{

    for (const auto &[key, value] : graph.edges)
        &edgesVector.emplace_back(key, value);
}

void convertVectorstoEdges(std::map<int32_t, std::vector<Edge>> &localEdgesMap, std::vector<pair<int, std::vector<Edge>>> &localEdgesVector)
{
    std::copy(localEdgesVector.begin(), localEdgesVector.end(),
              std::inserter(localEdgesMap, localEdgesMap.begin()));
}

void populateDisplacement(int total, int size, std::vector<int> &recvCount)
{
    for (int i = 0; i < size; ++i)
        recvCount[i] = (total / size) + (i < (total % size));
}

void boostSortNeighbours(std::vector<Edge> &e)
{
    boost::sort::block_indirect_sort(e.begin(), e.end(),
         [](const Edge &e1, const Edge &e2)
         {
             if (e1.source != e2.source)
                 return e1.source < e2.source;

             return e1.destination < e2.destination;
         });
}