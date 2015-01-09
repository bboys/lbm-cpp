// inspiration:
// https://github.com/anders-dc/lbm-d3q19/blob/master/lbm.c

#include "main.h"
#include <ctime>
#include <fstream>
#include <sstream>      // std::stringstream
using namespace Domains;

const size_t ITERATIONS = 1000000;
const size_t REPORT_PER_ITERATION = 50000;
const size_t dx = 20;
const size_t dy = 40;

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
    ss << "/" << ITERATIONS << "_" << iteration << ".txt";
    return ss.str();
}

int main(int argc, char **argv)
{
    auto start = std::clock();

    // Initialize a velocity set and a domain
    auto set = new D2Q9;
    auto domainSize = {dx, dy};

    // BoxedDomain initializer(set, domainSize);
    LidDrivenCavityDomain initializer(set, domainSize);

    // Create simulation
    LBM::Simulation sim(&initializer);
    for (size_t iter = 0; iter < ITERATIONS; ++iter)
    {
        sim.step();

        if (iter % REPORT_PER_ITERATION == 0)
        {
            // Create a output file
            std::ofstream out(createFileName(iter, "D2Q9", "TEST", domainSize), std::ios::out | std::ios::app);
            Reporting::MatlabReporter reporter(out);
            sim.report(reporter);

        }
        if (iter % 1000 == 0)
            std::cout << iter << " / " << ITERATIONS << '\n';
    }
    std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << '\n';

    std::ofstream out(createFileName(ITERATIONS + 1, "D2Q9", "LidDriven", domainSize), std::ios::out | std::ios::app);
    Reporting::MatlabReporter reporter(out);
    sim.report(reporter);


    // TODO: make a unique pointer of the set
    delete set;
    return 0;
}
