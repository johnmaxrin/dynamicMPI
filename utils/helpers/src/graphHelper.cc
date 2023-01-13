#include "../inc/boostMPIHelpers.hpp"

void sortNeighbours(std::vector<Edge> &e)
{
    sort(e.begin(), e.end(),
         [](const Edge &e1, const Edge &e2)
         {
             if (e1.source != e2.source)
                 return e1.source < e2.source;

             return e1.destination < e2.destination;
         });
}

void readFromFile(char *buffer, MPI_Offset readCount, int32_t &nodes, int32_t &edges, std::map<int32_t, std::vector<Edge>> &localedges, boost::mpi::communicator world)
{   
    nodes = 0;
    edges = 0;
    std::stringstream ss;
    ss.write(buffer, readCount);
    std::string str = ss.str();

    std::string line;

    while (std::getline(ss, line))
    {

        if (line.length() == 0 || line[0] < '0' || line[0] > '9')
            continue;

        std::stringstream ss(line);
        edges++;

        Edge edge;
        int32_t source;
        int32_t destination;
        int32_t weight;

        ss >> source;
        if (source > nodes)
            nodes = source;

        ss >> destination;
        if (destination > nodes)
            nodes = destination;

        edge.source = source;
        edge.destination = destination;
        edge.weight = 1;
        edge.id = source;

        ss >> weight;

        localedges[source].push_back(edge);
    }
}