#ifndef BOOST_MPI_HELPERS
#define BOOST_MPI_HELPERS

#include "../../../inc/mygraph.hpp"
#include <boost/sort/sort.hpp> 

void boostConvertEdgestoVector(std::vector<std::pair<int, std::vector<Edge>>> &edgesVector, Graph &graph);
void boostConvertVectorstoEdges(std::map<int32_t, std::vector<Edge>> &localEdgesMap, std::vector<std::pair<int, std::vector<Edge>>> &localEdgesVector);

/* Add Documentation for this function */
void boostPopulateDisplacement(int total, int size, std::vector<int> &recvCount);


void boostSortNeighbours(std::vector<Edge> &e);

void boostFileIO(boost::mpi::communicator, std::string, MPI_File &, MPI_Offset &, MPI_Offset &, int32_t &, int32_t &, std::map<int32_t,std::vector<Edge>> &);


#endif