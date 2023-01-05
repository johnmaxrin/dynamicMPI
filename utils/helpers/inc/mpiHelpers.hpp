#ifndef MPI_HELPERS
#define MPI_HELPERS

#include<mpi.h>
#include<vector>
#include<utility>
#include <cstddef>
#include<stddef.h>
#include"../../../inc/mygraph.hpp"


#define EDGECLASSELEMENTSCOUNT 4
#define GRAPHCLASSELEMENTSCOUNT 4
#define MAPPAIRCOUNT 2
#define VECTORCOUNT 2

int createMPIGraphDataType(MPI_Datatype *, Graph);
int destroyMPIGraphDataType(MPI_Datatype *);
void STARPLAT_MPI_SORT_NEIGHBOURS();




#endif