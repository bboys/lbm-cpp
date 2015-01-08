#include "LidDrivenCavity.h"

using namespace VelocitySets::D2Q9;
using namespace Reporting;

/*
* ______ +x
* |
* |
* |
* | +y
*
*
* ZZZZZ
* B   B
* B   B
* BBBBB
*/

namespace Domains { namespace LidDrivenCavity {

    bool periodic(size_t x, size_t y, size_t dx, size_t dy)
    {
        return ((x > 0 && x <= dx) && (y > 0 && y <= dy));
    }

    bool bounceback(size_t x, size_t y, size_t dx, size_t dy)
    {
        // Left and right solid walls
        return ((x == 0 || x == (dx - 1) || y == 0) &&
            (y != (dy - 1)  || x == 0 || x == (dx - 1)));

        //
    }


    bool zouHe(size_t x, size_t y, size_t dx, size_t dy)
    {
        return y == (dy - 1) && x != 0 && x != (dx - 1);
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
            // distributions[dir].value     = set.weights[dir];
            // distributions[dir].nextValue = set.weights[dir];
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
        // if (zouHe(x, y, dx, dy))
        //     nodes[idx].type = NodeType::ZouHe;
        // else if (bounceback(x, y, dx, dy))
        //     nodes[idx].type = NodeType::BounceBack;
        // else
        //     nodes[idx].type = NodeType::Periodic;

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

        for (size_t y = 0; y < dy; ++y)
            for (size_t x = 0; x < dx; ++x)
                initializeNodeAt(set, nodes[x + dx * y], x + dx * y);

        for (size_t y = 0; y < dy; ++y)
            for (size_t x = 0; x < dx; ++x)
                connectNodeToNeighbours(set, nodes, x, y, dx, dy);

        reportOnInitialSetup(set, nodes, dx, dy);

        return nodes;
    }

    // Call after fully initializing the domain
    BoundaryNode *boundaryNodes(Node *nodes, size_t dx, size_t dy, size_t &totalBoundaryNodes)
    {
        // Instead of boundaryNodes this should return PostProcessors

        // The top plane is a moving plate
        totalBoundaryNodes = dx - 2;
        // Node **zouHeNodes = new Node *[totalBoundaryNodes];
        BoundaryNode *zouHeNodes = new BoundaryNode[totalBoundaryNodes];
        size_t bIdx = 0;
        size_t totalNodes = dx * dy;

        Node *rightInterpolatedNode;
        Node *leftInterpolatedNode;
        size_t leftCorner = 0;
        size_t rightCorner = 0;

        for (size_t idx = 0; idx < totalNodes; ++idx)
        {
            if ((zouHe(nodes[idx].position[0], nodes[idx].position[1], dx, dy)))
            {
                zouHeNodes[bIdx].node = &nodes[idx];
                // Set the correct orientation
                if (nodes[idx].position[0] == 0)
                {
                    leftCorner = bIdx;
                    zouHeNodes[bIdx].direction = Direction::NW;
                }
                else if (nodes[idx].position[0] == dx - 1)
                {
                    rightCorner = bIdx;
                    zouHeNodes[bIdx].direction = Direction::NE;
                }
                else
                {
                    if (nodes[idx].position[0] == dx - 2)
                        leftInterpolatedNode = &nodes[idx];
                    else if (nodes[idx].position[0] == 1)
                        rightInterpolatedNode = &nodes[idx];

                    zouHeNodes[bIdx].direction = Direction::N;
                }

                ++bIdx;
            }
        }
        // We need to interpolate the density of the corner nodes
        zouHeNodes[rightCorner].interpolateFrom = leftInterpolatedNode;
        zouHeNodes[leftCorner].interpolateFrom = rightInterpolatedNode;

        return zouHeNodes;
    }

    // apply zou he etc.
    void applyBoundaryConditions(VelocitySet set, BoundaryNode *nodes, size_t totalBoundaryNodes)
    {
        double u_y = 0;
        double u_x = .05;
        double rho = 0; // density

        for (size_t idx = 0; idx < totalBoundaryNodes; ++idx)
        {
            Node current_node = *nodes[idx].node;
            switch (nodes[idx].direction)
            {
                // Left corner
                case Direction::NW:
                std::cout << "North West\t";
                    rho = density(set, *nodes[idx].interpolateFrom);
                    current_node.distributions[1].nextValue = current_node.distributions[3].nextValue;
                    current_node.distributions[4].nextValue = current_node.distributions[2].nextValue;
                    current_node.distributions[8].nextValue = current_node.distributions[6].nextValue;
                    current_node.distributions[5].nextValue = 0.5 * (rho - (
                        current_node.distributions[0].nextValue + current_node.distributions[1].nextValue +
                        current_node.distributions[2].nextValue + current_node.distributions[3].nextValue +
                        current_node.distributions[4].nextValue + current_node.distributions[6].nextValue +
                        current_node.distributions[8].nextValue
                    ));
                    current_node.distributions[7].nextValue = current_node.distributions[5].nextValue;
                break;
                // Right corner
                case Direction::NE:
                std::cout << "North East\t";
                    rho = density(set, *nodes[idx].interpolateFrom);
                    current_node.distributions[3].nextValue = current_node.distributions[1].nextValue;
                    current_node.distributions[4].nextValue = current_node.distributions[2].nextValue;
                    current_node.distributions[7].nextValue = current_node.distributions[5].nextValue;
                    current_node.distributions[6].nextValue = 0.5 * (rho - (
                        current_node.distributions[0].nextValue + current_node.distributions[1].nextValue +
                        current_node.distributions[2].nextValue + current_node.distributions[3].nextValue +
                        current_node.distributions[4].nextValue + current_node.distributions[5].nextValue +
                        current_node.distributions[7].nextValue
                    ));
                    current_node.distributions[8].nextValue = current_node.distributions[6].nextValue;
                break;
                // Top
                case Direction::N:
                // std::cout << "North\t";
                    rho = (1 / (1 + u_y)) * (
                        current_node.distributions[0].nextValue +
                        current_node.distributions[1].nextValue +
                        current_node.distributions[3].nextValue +
                        2 * (
                            current_node.distributions[2].nextValue +
                            current_node.distributions[5].nextValue +
                            current_node.distributions[6].nextValue
                        )
                    );

                    // current_node.distributions[4].nextValue = current_node.distributions[2].nextValue - 2/3 * (rho * u_y);
                    // current_node.distributions[7].nextValue = current_node.distributions[5].nextValue - (1/6) * (rho * u_y) -
                    //     0.5 * (current_node.distributions[3].nextValue - current_node.distributions[1].nextValue ) + 0.5 * (rho * u_x);
                    // current_node.distributions[8].nextValue = current_node.distributions[6].nextValue - (1/6) * (rho * u_y) +
                    //     0.5 * (current_node.distributions[3].nextValue - current_node.distributions[1].nextValue ) - 0.5 * (rho * u_x);


                    current_node.distributions[4].nextValue = current_node.distributions[2].nextValue;
                    current_node.distributions[7].nextValue  = current_node.distributions[5].nextValue +
                        0.5 *(current_node.distributions[1].nextValue - current_node.distributions[3].nextValue ) -
                        0.5 * rho * u_x;

                    current_node.distributions[8].nextValue = current_node.distributions[6].nextValue +
                        0.5 * (current_node.distributions[3].nextValue - current_node.distributions[1].nextValue) +
                        0.5 * rho * u_x;




                    // current_node.distributions[4].nextValue = current_node.distributions[2].nextValue - 2.0 / 3.0 * (rho * u_y);
                    // current_node.distributions[7].nextValue  = current_node.distributions[5].nextValue +
                    //     0.5 *(current_node.distributions[1].nextValue - current_node.distributions[3].nextValue ) -
                    //     0.5 * rho * u_x -
                    //     rho * u_y / 6.0;

                    // current_node.distributions[8].nextValue = current_node.distributions[6].nextValue +
                    //     0.5 * (current_node.distributions[3].nextValue - current_node.distributions[1].nextValue) +
                    //     0.5 * rho * u_x -
                    //     rho * u_y / 6.0;
                break;


                default:
                ;
            }

            /*// Bottom
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
                0.5 * (current_node.distributions[3].nextValue - current_node.distributions[1].nextValue ) - 0.5 * (density * u_x);*/


            // Top

            // ::Reporting::reportOnNode(set, node, 0, 0);
            // ::Reporting::reportOnDistributions(set, current_node);
        }
        // std::cout << '\n';
        return;
    }
}}



namespace Domains {
    LidDrivenCavityDomain::LidDrivenCavityDomain(VelocitySet &set, std::vector<size_t> domainSize)
    :
        DomainInitializer(set, domainSize)
    {}

    LidDrivenCavityDomain::~LidDrivenCavityDomain()
    {}

    void LidDrivenCavityDomain::connectNodeToNeighbours(Node &node)
    {
        size_t nDirections = d_set.nDirections;

        std::vector<int> position;
        for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
            position.push_back(node.position[dim]);

        for (size_t dir = 0; dir < nDirections; ++dir)
        {
            std::vector<int> neighbour;
            for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
                neighbour.push_back(
                    node.position[dim] + d_set.directions[dir][dim]
                );

            if (isZouHe(position) && !pointsOutwards(neighbour))
                node.distributions[dir].neighbour = nullptr;
            else if (isBounceBack(neighbour))
            {
                size_t op_dir = ::VelocitySets::D2Q9::oppositeDirectionOf(dir);
                node.distributions[dir].neighbour = &node.distributions[op_dir].nextValue;
            }
            else
            {
                // Periodic boundary
                for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
                    neighbour[dim] = (neighbour[dim] + d_domain_size[dim]) % d_domain_size[dim];

                size_t neighbour_idx = idxOf(neighbour);
                node.distributions[dir].neighbour = &d_nodes[neighbour_idx].distributions[dir].nextValue;
            }
        }
    }

    bool LidDrivenCavityDomain::isInDomain(std::vector<int> position)
    {
        // square domain
        return (
            position[0] >= 0 && position[0] < d_domain_size[0] &&
            position[1] >= 0 && position[1] < d_domain_size[1]
        );
    }

    bool LidDrivenCavityDomain::isBounceBack(std::vector<int> position)
    {
        return not isInDomain(position) && not isZouHe(position);
    }

    bool LidDrivenCavityDomain::isZouHe(std::vector<int> position)
    {
        return position[1] == 0; // top wall
    }

    bool LidDrivenCavityDomain::pointsOutwards(std::vector<int> position)
    {
        return position[1] < 0; // anything above our top wall is not periodic
    }

    void LidDrivenCavityDomain::createBoundaryNodes()
    {

    }

    BoundaryNode &LidDrivenCavityDomain::initializeBoundaryNode(Node &node)
    {

    }
}