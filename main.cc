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

int main(int argc, char **argv)
{
    auto start = std::clock();

    // Initialize a velocity set and a domain
    D2Q9 *set = new D2Q9;
    std::vector<size_t> domainSize = {dx, dy};
    BoxedDomain initializer(set, domainSize);

    // Create simulation
    LBM::Simulation sim(&initializer);
    for (size_t iter = 0; iter < ITERATIONS; ++iter)
    {
        sim.run();

        if (iter % REPORT_PER_ITERATION == 0)
        {
            std::stringstream ss;
            ss << "logs/D2Q9_BOXED_" << dx << 'x' << dy << "_" << iter << ".txt";
            std::string outputFileName = ss.str();
            std::ofstream out(outputFileName, std::ios::out | std::ios::app);
            Reporting::MatlabReporter reporter(out);
            sim.report(reporter);
        }
    }

    std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << '\n';


    // TODO: make a unique pointer of the set
    delete set;
    return 0;
}
