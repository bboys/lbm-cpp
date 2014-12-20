#include "periodic.h"

bool periodic(size_t x, size_t y, size_t dx, size_t dy)
{
    return ((x > 0 && x < dx) && (y > 0 && y < dy));
}

bool bounceback(size_t x, size_t y, size_t dx, size_t dy)
{
    return (not ((x > 0 && x < dx) && (y > 0 && y < dy)));
}

void initializeNodeAt(VelocitySet &set, Node &node)
{
    if (set.nDimensions != 2)
        throw "Dimensie van velocity set is niet 2";
    size_t nDirections = set.nDirections;
    node.type = Cell;

    Distribution *distributions = new Distribution[nDirections];
    for (size_t dir = 0; dir < nDirections; ++dir)
    {
        distributions[dir].value     = set.weights[dir];
        distributions[dir].nextValue = set.weights[dir];
    }
    node.distributions = distributions;
}

void connectNodeToNeighbours(VelocitySet &set, Node *nodes, size_t x, size_t y, size_t dx, size_t dy)
{
    if (set.nDimensions != 2)
        throw "Dimensie van velocity set is niet 2";
    size_t idx = x * dx + y;
    size_t nDirections = set.nDirections;

    for (size_t dir = 0; dir < nDirections; ++dir)
    {
        // size_t neighbour_x = x + set.directions[dir][0];
        // size_t neighbour_y = y + set.directions[dir][1];
        // TODO: Check if neighbour node direciton is a boundary
        // then do boundary stuff
        // otherwise do periodic stuff
        if (periodic(x, y, dx, dy))
        {
            std::cout << "Een periodic" << '\t';
            // periodic boundary
            size_t neighbour_x = x + set.directions[dir][0];
            size_t neighbour_y = y + set.directions[dir][1];
            size_t neighbour_idx = (neighbour_x % dx) * dx + (neighbour_y % dy);
            nodes[idx].distributions[dir].neighbour = &nodes[neighbour_idx].distributions[dir].nextValue;
        } else if (bounceback(x, y, dx, dy))
        {
            std::cout << "Een bounce back" << '\t';
            size_t neighbour_direction = oppositeDirectionOf(dir);
            nodes[idx].distributions[dir].neighbour = &nodes[idx].distributions[neighbour_direction].nextValue;
        }
    }
}

Node *initialize(VelocitySet &set, size_t &totalNodes)
{
    // Initialize the velocity set (note: should be available to all processors)
    initializeVelocitySet(set);

    size_t dx = 10;
    size_t dy = 10;

    totalNodes = dx * dy;
    Node *nodes = new Node[totalNodes];

    for (size_t x = 0; x < dx; ++x)
        for (size_t y = 0; y < dy; ++y)
            initializeNodeAt(set, nodes[x * dx + y]);

    for (size_t x = 0; x < dx; ++x)
        for (size_t y = 0; y < dy; ++y)
            connectNodeToNeighbours(set, nodes, x, y, dx, dy);

    return nodes;
}
