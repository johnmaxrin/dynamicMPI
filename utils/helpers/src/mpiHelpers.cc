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
    MPI_Datatype edgeType;
    int blocklengths[EDGECLASSELEMENTSCOUNT] = {1, 1, 1, 1};
    MPI_Datatype types[EDGECLASSELEMENTSCOUNT] = {MPI_INT32_T, MPI_INT32_T, MPI_INT32_T, MPI_INT32_T};
    MPI_Aint displacements[EDGECLASSELEMENTSCOUNT] = {offsetof(Edge, source), offsetof(Edge, destination), offsetof(Edge, weight), offsetof(Edge, id)};
    int res = MPI_Type_create_struct(4, blocklengths, displacements, types, &edgeType);
    if (res != MPI_SUCCESS) return res;

    // Creating Map::Vector<Edge> datatype
    MPI_Datatype mapType;
    int blocklenghts[MAPPAIRCOUNT] = {1,1}; 
    MPI_Datatype types[MAPPAIRCOUNT] = {MPI_INT32_T, edgeType};

    //To find the offset of second member
    MPI_Aint mapBase, vecBase;
    //MPI_Get_address(std::begin(Graph::getEdges()),&mapBase);
    //MPI_Get_address(std::begin(std::vector<Edge>()), &vecBase);



    return 0;
}


