#ifndef INCLUDED_MAIN
#define INCLUDED_MAIN

#include <iostream>
#include <string>

#include "VelocitySets/d2q9.h"

struct BounceBack;
struct Perriodic;
struct Source;
struct Node;
/*
struct StreamAble; // A structure to which we can stream,
// might be a Node, or a function to send the stream to a
// node on another processor
*/
enum BoundaryNode {
    BounceBack,
    Perriodic,
    Source
};

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

enum NodeType
{
    Cell,
    BoundaryNode
};

struct Node
{
    NodeType type;
    Distribution *distributions;
};

void initializeVelocitySet(VelocitySet & set);

#endif