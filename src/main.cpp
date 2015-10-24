#include "main.h"
#include <ctime>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Domains/DomainInitializer.h"
#include "Domains/BoxedDomain.h"
#include "Domains/LidDrivenCavity.h"
#include "Domains/PointDomain.h"
#include "VelocitySets/d2q9.h"

#include "LBM/node.h"
#include "LBM/parallel.h"

using namespace Domains;

// Global variables
size_t dx = 80;
size_t dy = 80;
size_t ITERATIONS;
size_t REPORT_PER_ITERATION = 10;
size_t P;

int main(int argc, char **argv)
try
{
    ITERATIONS = askForIterations(argc, argv);
    P          = askForProcessors(argc, argv);

    bsp_init(simulate, argc, argv);
    simulate();

    return 0;
}
catch(int x)
{
    return x;
}

void simulate()
{
    bsp_begin(P);

    size_t p = bsp_nprocs();
    size_t s = bsp_pid();

    if (s == 0)
        logSimulationData({dx, dy});

    bsp_sync();
    double initialization_time = bsp_time();
    // Initialize a velocity set and a domain
    D2Q9 set;
    auto domainSize = {dx, dy};

    DomainInitializer initializer(&set, domainSize, s, p);
    LBM::Simulation sim(&initializer);

    // Log initialization time and prepare computation time
    bsp_sync();
    double current_time = bsp_time();
    if (s == 0)
    {
        std::ofstream out("logs/timings.log", std::ios::out | std::ios::app);
        // Initialization time
        out << "IT: " << (current_time - initialization_time) << " sec, ";
    }
    double process_time = bsp_time();

    // Perform all iterations
    for (size_t iter = 0; iter < ITERATIONS; ++iter)
    {
        sim.step();

        if (iter % REPORT_PER_ITERATION == 0)
            sim.report();
    }

    // Create a timestamp
    bsp_sync();
    current_time = bsp_time();
    if (s == 0)
    {
        std::ofstream out("logs/timings.log", std::ios::out | std::ios::app);
        // computation time
        out << "CT: " << (current_time - process_time) << " sec" << '\n';
    }

    bsp_end();
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

    if (P > bsp_nprocs())
    {
        std::cout << "Sorry, not enough processors available." << std::endl;
        exit(0);
    }
    return P;
}

void logSimulationData(std::vector<size_t> domainSize)
{
    std::ofstream out("logs/timings.log", std::ios::out | std::ios::app);
    size_t p = bsp_nprocs();
    // Start by writing basic info to the file
    // out << "LBM simulation using " << p <<
    //     " processors to perform " << ITERATIONS << " iterations on the 'dummy' domain " <<
    //     ", with set: " << "D2Q9" << " and domain size: (";
    out << "LBM, p: " << p << ", it: " << ITERATIONS << ", ds (";
    showVector(domainSize, out);
    out << ", ";
}

void showVector(std::vector<size_t> vector, std::ofstream &out)
{
    for (size_t dim = 0; dim < (vector.size() - 1); ++dim)
        out << vector[dim] << ", ";
    out << vector[vector.size() - 1] << ")";
}

void createMatlabReport(LBM::Simulation &sim, size_t iter, std::vector<size_t> domainSize)
{
    // Create a output file
    std::ofstream out(createFileName(iter, "D2Q9", "PERIODIC", domainSize), std::ios::out | std::ios::app);
    Reporting::MatlabReporter reporter(out);
    sim.report(reporter);
}

std::string createFileName(size_t iteration, std::string setName, std::string domainName, std::vector<size_t> domainSize)
{
    // (iter, "D2Q9", "Lid_Driven_Cavity_5000_", domainSize);
    // logs/D2Q9_Lid_Driven_Cavity_5000_50_100
    // logs/D2Q9_Boxed_10x10_100.txt
    std::stringstream ss;
    ss << "logs/" << setName << "_" << domainName << "_";
    for (size_t idx = 0; idx < domainSize.size() - 1; ++idx)
        ss << domainSize[idx] << "x";
    ss << domainSize[domainSize.size() - 1];
    ss << "_" << ITERATIONS << "_" << iteration << ".txt";
    return ss.str();
}