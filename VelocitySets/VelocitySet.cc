#include "VelocitySet.h"
#include "../config.h"

// namespace VelocitySets {
    VelocitySet::VelocitySet(MY_SIZE_T nDirections, MY_SIZE_T nDimensions)
    :
            nDirections(nDirections),
            nDimensions(nDimensions)
    {}

    VelocitySet::~VelocitySet()
    {}
// }