#ifndef INCLUDED_MAIN
#define INCLUDED_MAIN

#include <string>
#include <vector>

#include "LBM/Simulation.h"

int main(int argc, char **argv);
void simulate();

// Setup functions
size_t askForIterations(int argc, char** argv);
size_t askForProcessors(int argc, char** argv);

// Logging functions
void logSimulationData(std::vector<size_t> domainSize);
void showVector(std::vector<size_t> vector, std::ofstream &out);

// Reporting functions
void createMatlabReport(LBM::Simulation &sim, size_t iter, std::vector<size_t> domainSize);
std::string createFileName(size_t iteration, std::string setName, std::string domainName, std::vector<size_t> domainSize);


#endif