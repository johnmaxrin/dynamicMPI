#!/bin/bash
#PBS -o logfile.log
#PBS -e errorfile_slash.err
#PBS -l walltime=00:60:00
#PBS -l select=2:ncpus=32
#PBS -q rupesh_gpuq

module load openmpi316

#tpdir=`echo $PBS_JOBID | cut -f 1 -d .`
#tempdir=$HOME/scratch/job$tpdir
#mkdir -p $tempdir
#cd $tempdir

#Compilation not needed already done
mpicxx -std=c++17 -g -Wall  -I/lfs/usrhome/oth/rnintern/scratch/MPI_Comparison/boost/install_dir/include  $PBS_O_WORKDIR/main.cc   /lfs/usrhome/oth/rnintern/scratch/MPI_Comparison/boost/install_dir/lib/libboost_mpi.a /lfs/usrhome/oth/rnintern/scratch/MPI_Comparison/boost/install_dir/lib/libboost_serialization.a -o output

echo "Done Compiling"


mpirun -n 32 $PBS_O_WORKDIR/output /lfs/usrhome/oth/rnintern/scratch/robert/StarPlatMPI/GraphDSL/Playground/DynamicMPI/manual/pokecud.txt >output.txt

#mv ../job$tpdir $PBS_O_WORKDIR/.

