#include"../inc/mpiHelpers.hpp"
#include "../../../inc/mygraph.hpp"

/*
const int num_fields = 2;
int blocklengths[num_fields] = {1, 1};
MPI_Datatype types[num_fields] = {MPI_INT, MPI_DOUBLE};
MPI_Aint offsets[num_fields];
offsets[0] = offsetof(MyClass, a);
offsets[1] = offsetof(MyClass, b);
MPI_Datatype mpi_type;
MPI_Type_create_struct(num_fields, blocklengths, offsets, types, &mpi_type);
MPI_Type_commit(&mpi_type);
*/



int createMPIGraphDataType(MPI_Datatype *graph)
{
    // Creating the edge datatype.
    int blocklengths[EDGECLASSELEMENTSCOUNT] = {1, 1, 1, 1};
    MPI_Aint displacements[EDGECLASSELEMENTSCOUNT] = {offsetof(Edge, source), offsetof(Edge, destination), offsetof(Edge, weight), offsetof(Edge, id)};
    MPI_Datatype types[EDGECLASSELEMENTSCOUNT] = {MPI_INT32_T, MPI_INT32_T, MPI_INT32_T, MPI_INT32_T};
    MPI_Datatype edge_type;
    int res = MPI_Type_create_struct(4, blocklengths, displacements, types, graph);
    printf("Successfully created Edge Data type \n");
    if (res != MPI_SUCCESS) return res;

    // Creating Map::Vector<Edge> datatype
    MPI_Datatype map_type;
    int blocklenghts[] = {1,1};
    //MPI_Aint offsets[] = {offsetof(std::pair<int, std::vector<Edge>>, first),offsetof(std::pair<int, std::vector<Edge>>, second)};


    return 0;
}


