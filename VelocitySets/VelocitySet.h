#ifndef INCLUDED_VELOCITY_SETS_VELOCITY_SET
#define INCLUDED_VELOCITY_SETS_VELOCITY_SET

#include "../config.h"
#include <cstring>
#include <vector>

// namespace VelocitySets {
    class VelocitySet
    {
        protected:
            std::vector<double> d_weights;
            std::vector<std::vector<int>> d_directions;
            double d_speed_of_sound_squared;

        public:
            MY_SIZE_T const nDirections;
            MY_SIZE_T const nDimensions;

            VelocitySet(MY_SIZE_T nDirections, MY_SIZE_T nDimensions);
            virtual ~VelocitySet();
            double weight(MY_SIZE_T);
            std::vector<int> direction(MY_SIZE_T);
            double speedOfSoundSquared();
            virtual MY_SIZE_T oppositeDirectionOf(MY_SIZE_T dir) = 0;
    };

    inline double VelocitySet::weight(MY_SIZE_T idx)
    {
        return d_weights[idx];
    }

    inline std::vector<int> VelocitySet::direction(MY_SIZE_T idx)
    {
        return d_directions[idx];
    }

    // inline MY_SIZE_T VelocitySet::nDirections()
    // {
    //     return d_directions.size();
    // }

    inline double VelocitySet::speedOfSoundSquared()
    {
        return d_speed_of_sound_squared;
    }
// }

#endif