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