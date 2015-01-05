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
#include "Domains/periodic.h"
#include "VelocitySets/d2q9.h"
#include "Reporting/MatlabReporter.h"

// struct BounceBack;
// struct Periodic;
// struct Source;
// struct Node;

// enum class Direction {
//     N, NE, E, S, SE, SW, W, NW
// };

// // struct ZouHe;
// struct ZouHe {
//     Direction direction;
//     // enum type {
//     //     velocity,
//     //     density
//     // },
//     // enum value {
//     //     density,
//     //     double *velocity
//     // },

// };
// enum ZouHe {
//     N, NE, E, S, SE, SW, W, NW
// };
/*
struct StreamAble; // A structure to which we can stream,
// might be a Node, or a function to send the stream to a
// node on another processor
*/


// A velocity set is a set of vectors with each having an associated weight
struct VelocitySet
{
    double *weights;
    int **directions;
    size_t nDirections;
    size_t nDimensions;
    double speedOfSoundSquared;
};

struct Distribution
{
    double value;
    double nextValue;
    double * neighbour; // stream_destination , should point to Distribution.value
};

struct Node
{
    NodeType type;
    Distribution *distributions;
    size_t *position;
};

int main(int argc, char **argv);
void stream(VelocitySet &set, Node *nodes, size_t totalNodes);

void collision(VelocitySet &set, Node *nodes, size_t totalNodes);
void collideNode(VelocitySet &set, Node &node);

double *equilibrium(VelocitySet &set, Node node);
double *velocity(VelocitySet &set, Node node);
double density(VelocitySet &set, Node node);

#endif