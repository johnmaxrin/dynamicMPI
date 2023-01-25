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

void readFromFile(std::string filePath, int32_t &nodes, int32_t &edges, std::vector<Edge> &localedges)
{   
    nodes = 0;
    edges = 0;

    std::ifstream infile;
     infile.open(filePath);
     std::string line;


     while (std::getline(infile,line))
     {

      // std::stringstream(line);

       if (line.length()==0||line[0] < '0' || line[0] >'9') {
          continue;

	    	}

        std::stringstream ss(line);

        edges++;
         
        //edgesTotal++; //TO BE REMOVED 

        //edgesTotal++; //TO BE REMOVED


        Edge e;
        
    
        int32_t source;
        int32_t destination;
        int32_t weightVal;

           ss>>source; 
           if(source>nodes)
              nodes=source;

            ss>>destination;  
            if(destination>nodes)
               nodes=destination;  

           e.source=source;
           e.destination=destination;
           e.weight=1;
           e.id = source;
           
           ss >> weightVal;

        localedges.push_back(e);    
     }

     infile.close();
}