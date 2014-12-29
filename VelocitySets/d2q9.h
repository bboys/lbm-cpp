#ifndef INCLUDED_VELOCITY_SET_D2Q9
#define INCLUDED_VELOCITY_SET_D2Q9

#include "../main.h"

struct VelocitySet;

namespace VelocitySets { namespace D2Q9 {
    void initializeVelocitySet(VelocitySet & set);
    size_t oppositeDirectionOf(size_t dir);
}}

#endif