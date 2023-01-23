#include <boost/mpi.hpp>
#include <boost/serialization/string.hpp>
#include <vector>
#include <string>
#include <iostream>

int main(int argc, char *argv[]) {
    boost::mpi::environment env{argc, argv};
    boost::mpi::communicator world;
    std::vector<int> a = {1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> b;

    // Resize "b" to ensure it can hold the number of elements that will be scattered to it
    b.resize(a.size() / world.size());

    // Perform the scatter operation on process 0
    if (world.rank() == 0) {
        boost::mpi::scatter(world, a, b, 0);
    } else {
        boost::mpi::scatter(world, a, b, 0);
    }
}
