#ifndef DYNAMIC_MPI_GLOBAL
#define DYNAMIC_MPI_GLOBAL

#define COMMONWORLD MPI_COMM_WORLD


#define GENMPI(rank,size,world) \
    MPI_Comm_size(world, size); \
    MPI_Comm_rank(world, rank); \

#define MASTER 0

#define MPIBARRIER(world) MPI_Barrier(world);


#endif