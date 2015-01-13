#include "main.h"
#include <ctime>
#include <fstream>
#include <sstream>
using namespace Domains;

extern "C" {
    #include "mcbsp.h"
}

// Global variables
size_t dx;
size_t dy;
size_t ITERATIONS;
size_t REPORT_PER_ITERATION;
size_t P;

size_t askForProcessors(int argc, char** argv)
{
    size_t P;
    if (argc > 1) {
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

void createMatlabReport(LBM::Simulation &sim, size_t iter, std::vector<size_t> domainSize)
{
    // Create a output file
    std::ofstream out(createFileName(iter, "D2Q9", "PERIODIC", domainSize), std::ios::out | std::ios::app);
    Reporting::MatlabReporter reporter(out);
    sim.report(reporter);
}

void sim()
{
    bsp_begin(P);
    size_t p = bsp_nprocs();
    size_t s = bsp_pid();

    auto start = std::clock();
    // Initialize a velocity set and a domain
    auto set = new D2Q9;
    auto domainSize = {dx, dy};

    // LidDrivenCavityDomain initializer(set, domainSize);
    // PointDomain initializer(set, domainSize);
    // BoxedDomain initializer(set, domainSize, s, p);

    double initialization_time = bsp_time();
    DomainInitializer initializer(set, domainSize, s, p);
    // Create simulation
    LBM::Simulation sim(&initializer);

    bsp_sync();
    double current_time = bsp_time();

    if (s==0)
        std::cout << "Initialization time: " << (current_time - initialization_time) << " seconds" << '\n';

    bsp_sync();
    double process_time = bsp_time();
    for (size_t iter = 0; iter < ITERATIONS; ++iter)
    {
        if (iter % REPORT_PER_ITERATION == 0 && s == 0)
            std::cout << 100 * static_cast<double>(iter)/ITERATIONS << '%' << '\n';
        sim.step();
    }
    bsp_sync();
    current_time = bsp_time();

    if (s==0)
        std::cout << "Computation time: " << (current_time - process_time) << " seconds" << '\n';

    bsp_sync();
    if (s == 0)
        std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC) << " s" << '\n';

    // createMatlabReport(sim, ITERATIONS + 1, domainSize);
    // TODO: make a unique pointer of the set
    delete set;
}

int main(int argc, char **argv)
{
    ITERATIONS = 100;
    REPORT_PER_ITERATION = 10;
    dx = 10;
    dy = 10;

    bsp_init(sim, argc, argv);

    P = askForProcessors(argc, argv);
    sim();

    exit(0);

    return 0;
}
