#ifndef INCLUDED_VELOCITY_SETS_VELOCITY_SET
#define INCLUDED_VELOCITY_SETS_VELOCITY_SET

#include <cstring>
#include <vector>
// namespace VelocitySets {
//     class VelocitySet
//     {
//         std::vector<double> d_weights;
//         std::vector<std::vector<int>> d_directions;
//         double d_speed_of_sound_squared;

//         public:
//             VelocitySet();
//             virtual ~VelocitySet();
//             double weight(size_t);
//             std::vector<int> direction(size_t);
//             double speedOfSoundSquared();
//             virtual size_t oppositeDirectionOf(size_t dir);
//     };

//     inline double VelocitySet::weight(size_t idx)
//     {
//         return d_weights[idx];
//     }

//     inline std::vector<int> VelocitySet::direction(size_t idx)
//     {
//         return d_directions[idx];
//     }

//     inline double VelocitySet::speedOfSoundSquared()
//     {
//         return d_speed_of_sound_squared;
//     }
// }

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