#include "mpi_header2.hpp"

using namespace std;

int main(int argc, char * argv[])
{
    Graph g(argv[1]);
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;
    
	try{	    
    g.initGraph(world);

     } catch(boost::mpi::exception& e)
	{
		std::cerr << "An error occurred during the gather operation: " << e.what() << std::endl;
        std::cerr << "Error code: " << e.result_code() << " - " << e.routine() << std::endl;
	}

    return 0;
}


