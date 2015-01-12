// inspiration:
// https://github.com/anders-dc/lbm-d3q19/blob/master/lbm.c

#include "main.h"
#include <ctime>
#include <fstream>
#include <sstream>      // std::stringstream
using namespace Domains;

const size_t ITERATIONS = 10000;
const size_t REPORT_PER_ITERATION = 1;
const size_t dx = 100;
const size_t dy = 100;

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
    std::ofstream out(createFileName(iter, "D2Q9", "POINT_2", domainSize), std::ios::out | std::ios::app);
    Reporting::MatlabReporter reporter(out);
    sim.report(reporter);
}

int main(int argc, char **argv)
{
    auto start = std::clock();
    // Initialize a velocity set and a domain
    auto set = new D2Q9;
    auto domainSize = {dx, dy};
    // BoxedDomain initializer(set, domainSize);
    // LidDrivenCavityDomain initializer(set, domainSize);
    PointDomain initializer(set, domainSize);
    // Create simulation
    LBM::Simulation sim(&initializer);
    for (size_t iter = 0; iter < ITERATIONS; ++iter)
    {
        if (iter % 100 == 0)
        {
            double percentage = 100 * static_cast<double>(iter)/ITERATIONS;
            std::cout << percentage << '%' << '\t';
            sim.report();
        }

        if (iter % REPORT_PER_ITERATION == 0)
            createMatlabReport(sim, iter, domainSize);

        sim.step();
    }
    std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << '\n';

    createMatlabReport(sim, ITERATIONS + 1, domainSize);
    // TODO: make a unique pointer of the set
    delete set;
    return 0;
}
