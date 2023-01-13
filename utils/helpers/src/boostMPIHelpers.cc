#include "../inc/boostMPIHelpers.hpp"

using namespace std;

void boostConvertEdgestoVector(std::vector<std::pair<int, std::vector<Edge>>> &edgesVector, Graph &graph)
{

    for (const auto &[key, value] : graph.edges)
        &edgesVector.emplace_back(key, value);
}

void boostConvertVectorstoEdges(std::map<int32_t, std::vector<Edge>> &localEdgesMap, std::vector<pair<int, std::vector<Edge>>> &localEdgesVector)
{
    std::copy(localEdgesVector.begin(), localEdgesVector.end(),
              std::inserter(localEdgesMap, localEdgesMap.begin()));
}

void boostPopulateDisplacement(int total, int size, std::vector<int> &recvCount)
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

void boostFileIO(boost::mpi::communicator world, std::string graphFile, MPI_File &fh, MPI_Offset &readCount, MPI_Offset &offset, int32_t &nodes, int32_t &edges, std::map<int32_t,std::vector<Edge>> &localEdges)
{
    MPI_Offset fileSize;
    MPI_File_open(MPI_COMM_WORLD, graphFile.data(), MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    MPI_File_get_size(fh, &fileSize);
    offset = world.rank() * (fileSize / world.size());
    readCount = (world.rank() == world.size() - 1) ? fileSize - offset : fileSize / world.size();
    char *buffer = (char *)malloc(readCount);
    MPI_Status status;
    MPI_File_read_at_all(fh, offset, buffer, readCount, MPI_BYTE, &status);

    readFromFile(buffer, readCount,nodes, edges,localEdges,world);
}

