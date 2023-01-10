#ifndef BOOST_MPI_HELPERS
#define BOOST_MPI_HELPERS

#include "../../../inc/mygraph.hpp"

void convertEdgestoVector(std::vector<std::pair<int, std::vector<Edge>>> &edgesVector, Graph &graph);
void convertVectorstoEdges(std::map<int32_t, std::vector<Edge>> &localEdgesMap, std::vector<std::pair<int, std::vector<Edge>>> &localEdgesVector);
void populateDisplacement();

#endif