#ifndef INCLUDED_VELOCITY_SETS_VELOCITY_SET
#define INCLUDED_VELOCITY_SETS_VELOCITY_SET

#include <cstring>

// A velocity set is a set of vectors with each having an associated weight
struct VelocitySet
{
    double *weights;
    int **directions;
    size_t nDirections;
    size_t nDimensions;
    double speedOfSoundSquared;
};

#endif