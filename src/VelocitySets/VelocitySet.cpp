#include "VelocitySet.h"

// namespace VelocitySets {
    VelocitySet::VelocitySet(size_t nDirections, size_t nDimensions)
    :
            nDirections(nDirections),
            nDimensions(nDimensions)
    {}

    VelocitySet::VelocitySet(
        std::vector<double> weights,
        std::vector<std::vector<int>> directions,
        double speed_of_sound_squared
    )
    :
        d_weights(weights),
        d_directions(directions),
        d_speed_of_sound_squared(speed_of_sound_squared),
        nDirections(d_directions.size()),
        nDimensions(d_directions[0].size())
    {
    }

    VelocitySet::~VelocitySet()
    {}
// }