// inspiration:
// https://github.com/anders-dc/lbm-d3q19/blob/master/lbm.c

#include "main.h"
#include <ctime>
#include <fstream>
#include <sstream>      // std::stringstream
using namespace Domains;

const size_t ITERATIONS = 10000;
const size_t REPORT_PER_ITERATION = 1000;
const size_t dx = 10;
const size_t dy = 10;

std::string createFileName(size_t iteration, std::string setName, std::string domainName, std::vector<size_t> domainSize)
{
    // logs/D2Q9_Boxed_10x10_100.txt
    std::stringstream ss;
    ss << "logs/" << setName << "_" << domainName << "_";
    for (size_t idx = 0; idx < domainSize.size() - 1; ++idx)
        ss << domainSize[idx] << "x";
    ss << domainSize[domainSize.size() - 1];
    ss << "_" << iteration << ".txt";
    return ss.str();
}

int main(int argc, char **argv)
{
    auto start = std::clock();

    // Initialize a velocity set and a domain
    auto set = new D2Q9;
    auto domainSize = {dx, dy};

    BoxedDomain initializer(set, domainSize);

    // Create simulation
    LBM::Simulation sim(&initializer);
    for (size_t iter = 0; iter < ITERATIONS; ++iter)
    {
        sim.step();

        if (iter % REPORT_PER_ITERATION == 0)
        {
            // Create a output file
            std::ofstream out(createFileName(iter, "D2Q9", "Boxed", domainSize), std::ios::out | std::ios::app);
            Reporting::MatlabReporter reporter(out);
            sim.report(reporter);
        }
    }
    std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << '\n';


    // TODO: make a unique pointer of the set
    delete set;
    return 0;
}
