#ifndef INCLUDED_LBM_BSP_PROGRAM
#define INCLUDED_LBM_BSP_PROGRAM

#include <cstdlib> // size_t
#include <vector>
#include <memory>

#include "Simulation.h"

#include "node.h"
#include "parallel_bsp.h"

class LBM_BSP_Program : public mcbsp::BSP_program {

    size_t p;
    size_t s;

    size_t d_iterations;
    std::vector<size_t> d_domainSize;

    public:

        LBM_BSP_Program(size_t iterations, std::vector<size_t> domainSize);

    protected:

        virtual void spmd();
        virtual BSP_program * newInstance();

    private:

        LBM::Simulation * setup_simulation();
        void run_simulation(LBM::Simulation * sim);
        void log_simulation_data();
        void show_vector(std::vector<size_t> vector, std::ofstream &out);

};


#endif