#ifndef INCLUDED_MAIN
#define INCLUDED_MAIN
/*
// #define PARALLEL (TODO)
#ifndef PARALLEL
    struct Messenger
    {
        double *source;
        size_t p;
        double *dest;
    };
    // als dest naar een distriubion wijst, dan hebben we hier geen nodes nodig
    communicate(Messenger *messengers, size_t totalMessengers);
#endif
*/
#include <iostream>
#include <string>

#include "Domains/LidDrivenCavity.h"
#include "VelocitySets/d2q9.h"
#include "Reporting/MatlabReporter.h"

#include "VelocitySets/VelocitySet.h"
#include "LBM/node.h"

// enum class Direction {
//     N, NE, E, S, SE, SW, W, NW
// };

// };
// enum ZouHe {
//     N, NE, E, S, SE, SW, W, NW
// };
/*
struct StreamAble; // A structure to which we can stream,
// might be a Node, or a function to send the stream to a
// node on another processor
*/

int main(int argc, char **argv);
void stream(VelocitySet &set, Node *nodes, size_t totalNodes);

void collision(VelocitySet &set, Node *nodes, size_t totalNodes);
void collideNode(VelocitySet &set, Node &node);

#endif