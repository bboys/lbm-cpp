#include "d2q9.h"

void initializeVelocitySet(VelocitySet & D2Q9)
{
    size_t dimensions = 2;
    size_t nDirections = 9;

    double *weights = new double[nDirections];
    weights[0] = 4.0 / 9;
    weights[1] = 1.0 / 9;
    weights[2] = 1.0 / 9;
    weights[3] = 1.0 / 9;
    weights[4] = 1.0 / 9;
    weights[5] = 1.0 / 36;
    weights[6] = 1.0 / 36;
    weights[7] = 1.0 / 36;
    weights[8] = 1.0 / 36;

    int **directions = new int*[nDirections];
    for(size_t i = 0; i < nDirections; ++i)
        directions[i] = new int[dimensions];
    // size_t directions[nDirections][2];
    directions[0][0] = 0;     directions[0][1] =   0;
    directions[1][0] = 1;     directions[1][1] =   0;
    directions[2][0] = 0;     directions[2][1] =   1;
    directions[3][0] = -1;    directions[3][1] =  0;
    directions[4][0] = 0;     directions[4][1] =  -1;
    directions[5][0] = 1;     directions[5][1] =    1;
    directions[6][0] = -1;    directions[6][1] =   1;
    directions[7][0] = -1;    directions[7][1] =  -1;
    directions[8][0] = 1;     directions[8][1] =   -1;

    D2Q9.weights    = weights;
    D2Q9.directions = directions;
    D2Q9.nDimensions = dimensions;
    D2Q9.nDirections = nDirections;

    std::cout << "This velocity set has " << D2Q9.nDimensions << " dimensions and "
            << D2Q9.nDirections << " directions." << '\n';

    for (size_t i = 0; i < nDirections; ++i)
        std::cout << D2Q9.weights[i] << ',' << D2Q9.directions[i][0] << ',' << D2Q9.directions[i][1] << '\n';
}


