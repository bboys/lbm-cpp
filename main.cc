#include "main.ih"

struct BounceBack;
struct Perriodic;
struct Source;
struct Node;
/*
struct StreamAble; // A structure to which we can stream,
// might be a Node, or a function to send the stream to a
// node on another processor
*/
enum BoundaryNode {
    BounceBack,
    Perriodic,
    Source
};

// A velocity set is a set of vectors with each having an associated weight
struct VelocitySet
{
    double *weights;
    int **directions;
};

struct Distribution
{
    double value;
    double * neighbour; // stream_destination , should point to Distribution.value
};

enum NodeType
{
    Cell,
    BoundaryNode
};

struct Node
{
    NodeType type;
    Distribution *distributions;
};


bool isOnBoundary(size_t x, size_t y, size_t z)
{
    if (x == 0 && y == 0) {
        return true;
    }
    return false;
}


void collision()
{

}

void stream()
{

}

void initializeVelocitySet(VelocitySet & D2Q9, size_t &dimensions)
{
    double *weights = new double[9];
    weights[0] = 4.0 / 9;
    weights[1] = 1.0 / 9;
    weights[2] = 1.0 / 9;
    weights[3] = 1.0 / 9;
    weights[4] = 1.0 / 9;
    weights[5] = 1.0 / 36;
    weights[6] = 1.0 / 36;
    weights[7] = 1.0 / 36;
    weights[8] = 1.0 / 36;

    dimensions = 2;
    size_t nDirections = 9;
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

    std::cout << "Velocity set:" << '\n';
    for (size_t i = 0; i < nDirections; ++i)
    {
        std::cout << D2Q9.weights[i] << ',' << D2Q9.directions[i][0] << ',' << D2Q9.directions[i][1] << '\n';
    }

}

Node *init(VelocitySet &velocitySet, size_t &dimensions, size_t &totalNodes)
{
    // Initialize the velocity set (note: should be available to all processors)
    initializeVelocitySet(velocitySet, dimensions);
    size_t dx = 10;
    size_t dy = 10;

    totalNodes = dx * dy;
    Node *nodes = new Node[totalNodes];
    for (size_t i = 0; i < totalNodes; ++i)
    {
        nodes[i].type = Cell;

        Distribution *distributions = new Distribution[9];
        for (size_t j = 0; j < 9; ++j)
        {
            distributions[j].value = velocitySet.weights[j];
            distributions[j].neighbour = &nodes[i % 5].distributions[j].value;
        }
        nodes[i].distributions = distributions;
    }

    std::cout << "Done" << '\n';

    return nodes;
}


int main(int argc, char **argv)
{
    // A 2d lattice boltzmann simulation
    VelocitySet VSet;
    Node *nodes;
    size_t totalNodes = 0;
    size_t dimensions = 0;
    nodes = init(VSet, dimensions, totalNodes);
    std::cout << "Hoi" << '\n';
    // for (int i = 0; i < totalNodes; ++i)
    // {
    //     std::cout << nodes[i].type << '\t';
    //     std::cout << nodes[i].distributions[0].value << '\n';
    // }

    size_t iterations = 1000;
    for (size_t iter = 0; iter < iterations; ++iter)
    {
        collision();
        stream();

        // if (iter % 100 == 0)
        //     report();
    }

    // Free up the memory taken by our velocity set
    delete[] VSet.weights;
    for (size_t i = 0; i < dimensions; ++i)
        delete[] VSet.directions[i];
    delete[] VSet.directions;

    for (size_t i = 0; i < totalNodes; ++i)
        delete[] nodes[i].distributions;
    delete[] nodes;

    return 0;
}
