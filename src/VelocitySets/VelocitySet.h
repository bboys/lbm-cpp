#ifndef INCLUDED_VELOCITY_SETS_VELOCITY_SET
#define INCLUDED_VELOCITY_SETS_VELOCITY_SET

#include <cstring>
#include <vector>

// namespace VelocitySets {
    class VelocitySet
    {
        protected:
            std::vector<double>           d_weights;
            std::vector<std::vector<int>> d_directions;
            double d_speed_of_sound_squared;

        public:
            size_t const nDirections;
            size_t const nDimensions;

            VelocitySet(size_t nDirections, size_t nDimensions);
            VelocitySet(std::vector<double> weights, std::vector<std::vector<int>> directions, double speed_of_sound_squared);
            virtual ~VelocitySet();
            double weight(size_t);
            std::vector<int> direction(size_t);
            double speedOfSoundSquared();
            virtual size_t oppositeDirectionOf(size_t dir) = 0;
    };

    inline double VelocitySet::weight(size_t idx)
    {
        return d_weights[idx];
    }

    inline std::vector<int> VelocitySet::direction(size_t idx)
    {
        return d_directions[idx];
    }

    // inline size_t VelocitySet::nDirections()
    // {
    //     return d_directions.size();
    // }

    inline double VelocitySet::speedOfSoundSquared()
    {
        return d_speed_of_sound_squared;
    }
// }

#endif