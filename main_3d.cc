/*#include "main.ih"

class Cell
{

    size_t * d_distributions;
    size_t * d_new_distributions;
    size_t * d_distribution_weights;

public:
    Cell();
    ~Cell();

    initializeDistributions(size_t *distributions, Cell *neighbours);

    stream();
    collide();
};

struct Type
{
    int Node;
    int BounceBack;
};

struct Distribution
{
    size_t value;
    Distribution * neighbour;
};

struct Cell
{
    Type type;
    Distribution distributions[9];
};

bool isOnBoundary(x, y, z)
{
    if (x == 0 && y == 0) {
        return true;
    }
    return false;
}

void init()
{
    dx = 10;
    dy = 10;
    dz = 10;

    Cell cells[dx * dy * dz];

    for (int x = 0; x < dx; ++x)
    {
        for (int y = 0; y < dy; ++y)
        {
            for (int z = 0; z < dz; ++z)
            {
                if (isOnBoundary(x, y, z))
                    cells[x + dx * y + dx * dy * z].type = BounceBack;
                else
                {
                    cells[x + dx * y + dx * dy * z].type = Node;
                    cells[x + dx * y + dx * dy * z].distributions = 0;
                }
            }
        }
    }
}

void collision()
{

}

void stream()
{

}

int main(int argc, char **argv)
{
    // A 2d lattice boltzmann simulation
    init()

    for (size_t iter = 0; iter < iterations; ++iter)
    {
        collision();
        stream();

        // if (iter % 100 == 0)
        //     report();
    }
    return 0;
}
*/