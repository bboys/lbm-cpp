#include "d2q9.h"

// namespace VelocitySets {
    D2Q9::D2Q9()
    :
        VelocitySet(9, 2)
    {
        d_weights.push_back(4.0 / 9.0);
        d_weights.push_back(1.0 / 9.0);
        d_weights.push_back(1.0 / 9.0);
        d_weights.push_back(1.0 / 9.0);
        d_weights.push_back(1.0 / 9.0);
        d_weights.push_back(1.0 / 36.0);
        d_weights.push_back(1.0 / 36.0);
        d_weights.push_back(1.0 / 36.0);
        d_weights.push_back(1.0 / 36.0);

        //                 x                           y                    z?
        d_directions.push_back(std::vector<int> { 0,  0});
        d_directions.push_back(std::vector<int> { 1,  0});
        d_directions.push_back(std::vector<int> { 0,  1});
        d_directions.push_back(std::vector<int> {-1,  0});
        d_directions.push_back(std::vector<int> { 0, -1});
        d_directions.push_back(std::vector<int> { 1,  1});
        d_directions.push_back(std::vector<int> {-1,  1});
        d_directions.push_back(std::vector<int> {-1, -1});
        d_directions.push_back(std::vector<int> { 1, -1});

        d_speed_of_sound_squared = 1.0 / 3.0;
    }

    D2Q9::~D2Q9()
    {}
// }