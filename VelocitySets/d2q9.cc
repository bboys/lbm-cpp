#include "d2q9.h"

namespace VelocitySets { namespace OLD { namespace D2Q9 {

    size_t oppositeDirectionOf(size_t dir)
    {
        switch (dir)
        {
            case 0: return 0;
            case 1: return 3;
            case 2: return 4;
            case 3: return 1;
            case 4: return 2;
            case 5: return 7;
            case 6: return 8;
            case 7: return 5;
            case 8: return 6;
        }
        throw "Invalid direction";
    }

    void initializeVelocitySet(::VelocitySet & set)
    {
        size_t nDimensions = 2;
        size_t nDirections = 9;

        double *weights = new double[nDirections];
        weights[0] = 4.0 / 9.0;
        weights[1] = 1.0 / 9.0;
        weights[2] = 1.0 / 9.0;
        weights[3] = 1.0 / 9.0;
        weights[4] = 1.0 / 9.0;
        weights[5] = 1.0 / 36.0;
        weights[6] = 1.0 / 36.0;
        weights[7] = 1.0 / 36.0;
        weights[8] = 1.0 / 36.0;

        int **directions = new int*[nDirections];
        for(size_t i = 0; i < nDirections; ++i)
            directions[i] = new int[nDimensions];
        // size_t directions[nDirections][2];
        //                 x                           y                    z?
        directions[0][0] = 0;     directions[0][1] =   0;
        directions[1][0] = 1;     directions[1][1] =   0;
        directions[2][0] = 0;     directions[2][1] =   1;
        directions[3][0] = -1;    directions[3][1] =  0;
        directions[4][0] = 0;     directions[4][1] =  -1;
        directions[5][0] = 1;     directions[5][1] =    1;
        directions[6][0] = -1;    directions[6][1] =   1;
        directions[7][0] = -1;    directions[7][1] =  -1;
        directions[8][0] = 1;     directions[8][1] =   -1;

        set.weights    = weights;
        set.directions = directions;
        set.nDimensions = nDimensions;
        set.nDirections = nDirections;
        set.speedOfSoundSquared = 1.0 / 3.0;

        // std::cout << "This velocity set has " << D2Q9.nDimensions << " dimensions and "
        //         << D2Q9.nDirections << " directions and speed of sound squared: "
        //         << D2Q9.speedOfSoundSquared << "." << '\n';

        // for (size_t i = 0; i < nDirections; ++i)
        //     std::cout << D2Q9.weights[i] << ',' << D2Q9.directions[i][0] << ',' << D2Q9.directions[i][1] << '\n';
    }
}}}

// namespace VelocitySets {
//     D2Q9::D2Q9()
//     {

//     }

//     D2Q9::~D2Q9()
//     {}
// }