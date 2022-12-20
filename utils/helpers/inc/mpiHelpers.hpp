#ifndef MPI_HELPERS
#define MPI_HELPERS

#include<mpi.h>
#include <cstddef>


#define EDGECLASSELEMENTSCOUNT 4

int createMPIGraphDataType(MPI_Datatype *MPI_GRAPH);
void STARPLAT_MPI_SORT_NEIGHBOURS();




#endif