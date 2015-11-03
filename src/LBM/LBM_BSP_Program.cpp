#include "LBM_BSP_Program.h"

#include "../Domains/DomainInitializer.h"
#include "../VelocitySets/d2q9.h"

#include <ctime>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace Domains;
using std::make_unique;
using namespace mcbsp;

size_t REPORT_PER_ITERATION = 10;

LBM_BSP_Program::LBM_BSP_Program(size_t iterations, std::vector<size_t> domainSize)
{
    d_iterations = iterations;
    d_domainSize = domainSize;
}

void LBM_BSP_Program::spmd() {
    p = bsp_nprocs();
    s = bsp_pid();

    std::unique_ptr<LBM::Simulation> sim(setup_simulation());
    run_simulation(sim.get());
}


BSP_program * LBM_BSP_Program::newInstance() {
    return new LBM_BSP_Program(d_iterations, d_domainSize);
}

/**
 *
 * @param p amount of processors being used
 * @param s current processor index
 */
LBM::Simulation * LBM_BSP_Program::setup_simulation()
{
    if (s == 0)
        log_simulation_data();

    bsp_sync();
    double initialization_time = bsp_time();

    auto set = new D2Q9;
    DomainInitializer initializer(set, d_domainSize, s, p);

    auto sim = new LBM::Simulation(initializer.domain());

    // we'll send both the local idx and the direction which we we'll change
    MCBSP_BYTESIZE_TYPE tag_size = sizeof(size_t[2]);
    bsp_set_tagsize(&tag_size);

    // Log initialization time and prepare computation time
    bsp_sync();
    double current_time = bsp_time();
    if (s == 0)
    {
        std::ofstream out("logs/timings.log", std::ios::out | std::ios::app);
        // Initialization time
        out << "IT: " << (current_time - initialization_time) << " sec, ";
    }
    return sim;
}

/**
 * Run ITERATIONS step and periodically report the current state of the simulation
 * @param sim
 */
void LBM_BSP_Program::run_simulation(LBM::Simulation * sim)
{
    double process_time = bsp_time();

    for (size_t iter = 0; iter < d_iterations; ++iter)
    {
        sim->step();

        if (iter % REPORT_PER_ITERATION == 0)
            sim->report();
    }

    // Create a timestamp
    bsp_sync();
    double current_time = bsp_time();
    if (s == 0)
    {
        std::ofstream out("logs/timings.log", std::ios::out | std::ios::app);
        // computation time
        out << "CT: " << (current_time - process_time) << " sec" << '\n';
    }
}

void LBM_BSP_Program::log_simulation_data()
{
    std::ofstream out("logs/timings.log", std::ios::out | std::ios::app);
    size_t p = bsp_nprocs();
    // Start by writing basic info to the file
    out << "LBM, p: " << p << ", it: " << d_iterations << ", ds (";
    show_vector(d_domainSize, out);
    out << ", ";
}

void LBM_BSP_Program::show_vector(std::vector<size_t> vector, std::ofstream &out)
{
    for (size_t dim = 0; dim < (vector.size() - 1); ++dim)
        out << vector[dim] << ", ";
    out << vector[vector.size() - 1] << ")";
}