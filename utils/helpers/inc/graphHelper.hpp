/*

Provide the documentation here. 

*/


#ifndef GRAPH_HELPER
#define GRAPH_HELPER

#include "../../../inc/mygraph.hpp"

void sortNeighbours(std::vector<Edge> &e);
void readFromFile(std::string filePath, int32_t &nodes, int32_t &edges, std::vector<Edge> &localedges);
#endif