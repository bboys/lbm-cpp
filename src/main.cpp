#include "main.h"

#include <iostream>
#include <sstream>

#include "LBM/LBM_BSP_Program.h"

int main(int argc, char **argv)
{
    size_t iterations = askForIterations(argc, argv);
    size_t processors = askForProcessors(argc, argv);

    std::vector<size_t> domain_size = {10, 10};

    LBM_BSP_Program lbm_program(iterations, domain_size);
    lbm_program.begin(processors);

    return 0;
}

size_t askForIterations(int argc, char** argv)
{
    size_t iterations = 11;
    if (argc > 2) {
        std::istringstream iss(argv[2]);
        if ((iss >> iterations) )
            return iterations;
    }

    return iterations;
}

size_t askForProcessors(int argc, char** argv)
{
    size_t P;
    if (argc > 1)
    {
        std::istringstream iss(argv[1]);
        if ((iss >> P) )
            return P;
    }
    else
    {
        std::cout << "How many processors do you want to use?" << std::endl;
        std::cin >> P;
    }

    return P;
}
