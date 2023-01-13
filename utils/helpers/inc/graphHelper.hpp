/*

Provide the documentation here. 

*/


#ifndef GRAPH_HELPER
#define GRAPH_HELPER

#include "../../../inc/mygraph.hpp"

void sortNeighbours(std::vector<Edge> &e);
void readFromFile(char *buffer, MPI_Offset readCount, int32_t &nodes, int32_t &edges, std::map<int32_t, std::vector<Edge>> &localedges,boost::mpi::communicator world);
#endif