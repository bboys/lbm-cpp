#ifndef INCLUDED_LBM_NODE
#define INCLUDED_LBM_NODE

#include "../config.h"

#include <cstring>
#include "../VelocitySets/VelocitySet.h"

struct Distribution
{
    double value;
    double nextValue;
    double * neighbour; // stream_destination , should point to Distribution.value
};

struct Node
{
    Distribution *distributions;
    MY_SIZE_T *position;
};

double *equilibrium(VelocitySet *set, Node node);
double *velocity(VelocitySet *set, Node node);
double density(VelocitySet *set, Node node);

#endif