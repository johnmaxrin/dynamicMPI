#include "../inc/mpiHelpers.hpp"
#include "../../../inc/mygraph.hpp"


using namespace std;

// Declaring MPI Datatype varriables.

MPI_Datatype edgeType, vectorType, mapType, ccharType, pairType;

int createMPIGraphDataType(MPI_Datatype *graphType, Graph graph)
{
    // Creating the edge datatype.
    int edgeBlockLengths[EDGECLASSELEMENTSCOUNT] = {1, 1, 1, 1};
    MPI_Datatype edgeFieldTypes[EDGECLASSELEMENTSCOUNT] = {MPI_INT32_T, MPI_INT32_T, MPI_INT32_T, MPI_INT32_T};
    MPI_Aint edgeDisplacements[EDGECLASSELEMENTSCOUNT] = {offsetof(Edge, source), offsetof(Edge, destination), offsetof(Edge, weight), offsetof(Edge, id)};
    int res = MPI_Type_create_struct(4, edgeBlockLengths, edgeDisplacements, edgeFieldTypes, &edgeType);
    if (res != MPI_SUCCESS)
        return res;

    // Creating Vector<Edge> datatype
    MPI_Type_contiguous(1, edgeType, &vectorType);
    MPI_Type_commit(&vectorType);

    // Creating Pair<int,vector<Edge>>
    int pairBlockLengths[MAPPAIRCOUNT] = {1, 1};
    MPI_Aint pairDisplacements[MAPPAIRCOUNT];
    MPI_Datatype pairTypes[MAPPAIRCOUNT] = {MPI_INT, vectorType};
    std::pair<const int, std::vector<Edge>> pair;
    MPI_Get_address(&pair.first, &pairDisplacements[0]);
    MPI_Get_address(&pair.second, &pairDisplacements[1]);

    for (int i = 1; i >= 0; --i)
        pairDisplacements[i] -= pairDisplacements[0];

    MPI_Type_create_struct(2, pairBlockLengths, pairDisplacements, pairTypes, &pairType);
    MPI_Type_commit(&pairType);

    // Creating Map::Vector<Edge> datatype
    MPI_Type_contiguous(1, pairType, &mapType);
    MPI_Type_commit(&mapType);

    // Creating Char * datatype
    MPI_Type_contiguous(1, MPI_CHAR, &ccharType);
    MPI_Type_commit(&ccharType);

    // Creating Graph datatype
    int graphBlockLengths[GRAPHCLASSELEMENTSCOUNT] = {1, 1, 1, 1};
    MPI_Datatype graphFieldTypes[GRAPHCLASSELEMENTSCOUNT] = {MPI_INT32_T, MPI_INT32_T, ccharType, mapType};

    MPI_Aint graphDisplacements[GRAPHCLASSELEMENTSCOUNT];

    int32_t totalNodes = graph.getNodeCount();
    int32_t totalEdges = graph.getEdgeCount();
    const char *filename = graph.getFileName();
    std::map<int32_t, std::vector<Edge>> edges = graph.getEdges();

    MPI_Get_address(&totalNodes, &graphDisplacements[0]);
    MPI_Get_address(&totalEdges, &graphDisplacements[1]);
    MPI_Get_address(&filename, &graphDisplacements[2]);
    MPI_Get_address(&edges, &graphDisplacements[3]);

    for (int i = 3; i >= 0; --i)
        graphDisplacements[i] -= graphDisplacements[0];

    res = MPI_Type_create_struct(4, graphBlockLengths, graphDisplacements, graphFieldTypes, graphType);
    MPI_Type_commit(graphType);

    if (res != MPI_SUCCESS)
        return res;

    return 0;
}

int destroyMPIGraphDataType(MPI_Datatype *graphType)
{
    MPI_Type_free(graphType);
    MPI_Type_free(&edgeType);
    MPI_Type_free(&vectorType);
    MPI_Type_free(&mapType);
    MPI_Type_free(&ccharType);
    MPI_Type_free(&pairType);
    return 0;
}
