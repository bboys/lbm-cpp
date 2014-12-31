#ifndef INCLUDED_MAIN
#define INCLUDED_MAIN

#include <iostream>
#include <string>

#include "Domains/LidDrivenCavity.h"
#include "Domains/periodic.h"
#include "VelocitySets/d2q9.h"

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
    double *position;
};

int main(int argc, char **argv);
void stream(VelocitySet &set, Node *nodes, size_t totalNodes);

void collision(VelocitySet &set, Node *nodes, size_t totalNodes);
void collideNode(VelocitySet &set, Node &node);

double *equilibrium(VelocitySet &set, Node node);
double *velocity(VelocitySet &set, Node node);
double density(VelocitySet &set, Node node);

#endif