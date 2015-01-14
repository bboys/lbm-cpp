#include "config.h"
#include "main.h"
#include <ctime>
#include <fstream>
#include <sstream>
using namespace Domains;

#include "LBM/parallel.h"

// Global variables
MY_SIZE_T dx;
MY_SIZE_T dy;
MY_SIZE_T ITERATIONS;
MY_SIZE_T REPORT_PER_ITERATION;
MCBSP_PROCESSOR_INDEX_DATATYPE P;

MY_SIZE_T askForIterations(int argc, char** argv)
{
    MY_SIZE_T iterations = 1000;
    if (argc > 2) {
        std::istringstream iss(argv[2]);
        if ((iss >> iterations) )
            return iterations;
    }

    return iterations;
}

MCBSP_PROCESSOR_INDEX_DATATYPE askForProcessors(int argc, char** argv)
{
    MCBSP_PROCESSOR_INDEX_DATATYPE P;
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

std::string createFileName(MY_SIZE_T iteration, std::string setName, std::string domainName, std::vector<MY_SIZE_T> domainSize)
{
    // (iter, "D2Q9", "Lid_Driven_Cavity_5000_", domainSize);
    // logs/D2Q9_Lid_Driven_Cavity_5000_50_100
    // logs/D2Q9_Boxed_10x10_100.txt
    std::stringstream ss;
    ss << "logs/" << setName << "_" << domainName << "_";
    for (MY_SIZE_T idx = 0; idx < domainSize.size() - 1; ++idx)
        ss << domainSize[idx] << "x";
    ss << domainSize[domainSize.size() - 1];
    ss << "_" << ITERATIONS << "_" << iteration << ".txt";
    return ss.str();
}


void createMatlabReport(LBM::Simulation &sim, MY_SIZE_T iter, std::vector<MY_SIZE_T> domainSize)
{
    // Create a output file
    std::ofstream out(createFileName(iter, "D2Q9", "PERIODIC", domainSize), std::ios::out | std::ios::app);
    Reporting::MatlabReporter reporter(out);
    sim.report(reporter);
}

void showVector(std::vector<MY_SIZE_T> vector, std::ofstream &out)
{
    for (MY_SIZE_T dim = 0; dim < (vector.size() - 1); ++dim)
        out << vector[dim] << ", ";
    out << vector[vector.size() - 1] << ")";
}

void logSimulationData(std::vector<MY_SIZE_T> domainSize)
{
    std::ofstream out("logs/timings.log", std::ios::out | std::ios::app);
    MY_SIZE_T p = bsp_nprocs();
    // Start by writing basic info to the file
    // out << "LBM simulation using " << p <<
    //     " processors to perform " << ITERATIONS << " iterations on the 'dummy' domain " <<
    //     ", with set: " << "D2Q9" << " and domain size: (";
    out << "LBM, p: " << p << ", it: " << ITERATIONS << ", ds (";
    showVector(domainSize, out);
    out << ", ";
}

void simulate()
{
    bsp_begin(P);

    MY_SIZE_T p = bsp_nprocs();
    MY_SIZE_T s = bsp_pid();

    if (s == 0)
        logSimulationData({dx, dy});

    bsp_sync();
    double initialization_time = bsp_time();
    // Initialize a velocity set and a domain
    D2Q9 set;
    auto domainSize = {dx, dy};

    // All domains except DomainInitializer are currently not working!
        // LidDrivenCavityDomain initializer(&set, domainSize);
        // PointDomain initializer(&set, domainSize);
        // BoxedDomain initializer(&set, domainSize, s, p);
    DomainInitializer initializer(&set, domainSize, s, p);

    // Create simulation
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
    for (MY_SIZE_T iter = 0; iter < ITERATIONS; ++iter)
        sim.step();

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

int main(int argc, char **argv)
{
    ITERATIONS = askForIterations(argc, argv);
    REPORT_PER_ITERATION = 10;
    dx = 80;
    dy = 80;

    bsp_init(simulate, argc, argv);

    P = askForProcessors(argc, argv);
    simulate();

    return 0;
}
