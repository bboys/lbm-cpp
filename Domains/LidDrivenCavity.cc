#include "LidDrivenCavity.h"

using namespace VelocitySets::D2Q9;
using namespace Reporting;

namespace Domains { namespace LidDrivenCavity {

    bool periodic(size_t x, size_t y, size_t dx, size_t dy)
    {
        return ((x > 0 && x <= dx) && (y > 0 && y <= dy));
    }

    bool bounceback(size_t x, size_t y, size_t dx, size_t dy)
    {
        // Left and right solid walls
        return ((x == 0 || x == (dx - 1) || y == (dy - 1)) && y != 0);
    }

    bool zouHe(size_t x, size_t y, size_t dx, size_t dy)
    {
        return y == 0;
    }

    bool points_outwards(size_t x, size_t y, size_t dx, size_t dy)
    {
        return (x < 0 || x >= dx || y < 0 || y >= dy);
    }

    void initializeNodeAt(VelocitySet &set, Node &node, size_t idx)
    {
        if (set.nDimensions != 2)
            throw "Dimensie van velocity set is niet 2";
        size_t nDirections = set.nDirections;

        Distribution *distributions = new Distribution[nDirections];
        for (size_t dir = 0; dir < nDirections; ++dir)
        {
            // Commented distributions are here for easy testing purposes
            distributions[dir].value     = idx * 10 + dir;//set.weights[dir];
            distributions[dir].nextValue = -1;//idx * 10 + dir;//set.weights[dir];
            distributions[dir].value     = set.weights[dir];
            distributions[dir].nextValue = set.weights[dir];
        }
        node.distributions = distributions;
    }

    void connectNodeToNeighbours(VelocitySet &set, Node *nodes, size_t x, size_t y, size_t dx, size_t dy)
    {
        if (set.nDimensions != 2)
            throw "Dimensie van velocity set is niet 2";

        size_t idx = x + dx * y;
        size_t nDirections = set.nDirections;
        size_t nDimensions = set.nDimensions;
        nodes[idx].position = new size_t[nDimensions];
        nodes[idx].position[0] = x;
        nodes[idx].position[1] = y;

        // note: this is not the right place for this. BounceBack nodes should not be included in storage since we use pointers
        // to stream
        if (zouHe(x, y, dx, dy))
            nodes[idx].type = NodeType::ZouHe;
        else if (bounceback(x, y, dx, dy))
            nodes[idx].type = NodeType::BounceBack;
        else
            nodes[idx].type = NodeType::Periodic;

        for (size_t dir = 0; dir < nDirections; ++dir)
        {
            // TODO: Check if neighbour node direciton is a boundary
            // then do boundary stuff
            // otherwise do periodic stuff
            int neighbour_x = x + set.directions[dir][0];
            int neighbour_y = y + set.directions[dir][1];
            if (zouHe(x, y, dx, dy) && points_outwards(neighbour_x, neighbour_y, dx, dy))
            {
                // Note this should actually be a nullpointer, but then we would have to rewrite the streaming function
                nodes[idx].distributions[dir].neighbour = nullptr;
            }
            else if (bounceback(neighbour_x, neighbour_y, dx, dy))
            {
                // bounce back (no slip)
                size_t op_dir = oppositeDirectionOf(dir);
                nodes[idx].distributions[dir].neighbour = &nodes[idx].distributions[op_dir].nextValue;
            }
            else
            {
                // periodic
                // add dx and dy to the modulo statements to make it positive
                size_t neighbour_idx = ((neighbour_x + dx) % dx) + dx * ((neighbour_y + dy) % dy);
                nodes[idx].distributions[dir].neighbour = &nodes[neighbour_idx].distributions[dir].nextValue;
            }
        }
    }

    Node *initialize(VelocitySet &set, size_t &totalNodes, size_t dx, size_t dy)
    {
        // Initialize the velocity set (note: should be available to all processors)
        initializeVelocitySet(set);

        totalNodes = dx * dy;
        Node *nodes = new Node[totalNodes];

        for (size_t x = 0; x < dx; ++x)
            for (size_t y = 0; y < dy; ++y)
                initializeNodeAt(set, nodes[x + dx * y], x + dx * y);

        for (size_t x = 0; x < dx; ++x)
            for (size_t y = 0; y < dy; ++y)
                connectNodeToNeighbours(set, nodes, x, y, dx, dy);

        reportOnInitialSetup(set, nodes, dx, dy);

        return nodes;
    }

    // Call after fully initializing the domain
    BoundaryNode *boundaryNodes(Node *nodes, size_t const dx, size_t dy, size_t &totalBoundaryNodes)
    {
        // The top plane is a moving plate
        totalBoundaryNodes = dy;
        // Node **zouHeNodes = new Node *[totalBoundaryNodes];
        BoundaryNode *zouHeNodes = new BoundaryNode[totalBoundaryNodes];
        size_t bIdx = 0;
        size_t totalNodes = dx * dy;

        for (size_t idx = 0; idx < totalNodes; ++idx)
        {
            if (nodes[idx].type == NodeType::ZouHe)
            {
                zouHeNodes[bIdx].node = &nodes[idx];
                // Set the correct orientation
                if (nodes[idx].position[0] == 0)
                    zouHeNodes[bIdx].direction = Direction::NW;
                else if (nodes[idx].position[0] == dx - 1)
                    zouHeNodes[bIdx].direction = Direction::NE;
                else
                    zouHeNodes[bIdx].direction = Direction::N;

                ++bIdx;
            }
        }

        return zouHeNodes;
    }

    // apply zou he etc.
    void applyBoundaryConditions(VelocitySet set, BoundaryNode *nodes, size_t totalBoundaryNodes)
    {
        double u_y = 0;
        double u_x = .05;
        double density = 0;

        for (size_t idx = 0; idx < totalBoundaryNodes; ++idx)
        {
            // switch (nodes[idx].direction)
            // {
            //     case Direction::NW: std::cout << "NW\t"; break;
            //     case Direction::NE: std::cout << "NE\t"; break;
            //     case Direction::N: std::cout << "N\t"; break;


            //     default: std::cout << "Iets anders" << '\n';
            // }
            Node current_node = *nodes[idx].node;
            density = (1 / (1 + u_y)) * (
                current_node.distributions[0].nextValue +
                current_node.distributions[1].nextValue +
                current_node.distributions[3].nextValue +
                2 * (
                    current_node.distributions[4].nextValue +
                    current_node.distributions[7].nextValue +
                    current_node.distributions[8].nextValue
                )
            );

            current_node.distributions[2].nextValue = current_node.distributions[4].nextValue + 2/3 * (density * u_y);
            current_node.distributions[5].nextValue = current_node.distributions[7].nextValue + (1/6) * (density * u_y) +
                0.5 * (current_node.distributions[3].nextValue - current_node.distributions[1].nextValue ) + 0.5 * (density * u_x);
            current_node.distributions[6].nextValue = current_node.distributions[8].nextValue + (1/6) * (density * u_y) -
                0.5 * (current_node.distributions[3].nextValue - current_node.distributions[1].nextValue ) - 0.5 * (density * u_x);
            // ::Reporting::reportOnNode(set, node, 0, 0);
            // ::Reporting::reportOnDistributions(set, current_node);
        }
        // std::cout << '\n';
        return;
    }
}}