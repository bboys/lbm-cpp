#include "Simulation.h"
#include "node.h"
#include <iostream>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>

#include "../Reporting/MatlabReporter.h"

#include "../LBM/parallel_bsp.h"

namespace LBM {

    void Simulation::report(::Reporting::MatlabReporter reporter)
    {
        // reporter.reportOnTimeStep(d_domain->set, d_domain->nodes);
    }

    void Simulation::report()
    {

        for (size_t p = 0; p < bsp_nprocs(); ++p)
        {
            if (p == bsp_pid())
            {
                std::ofstream out("logs/test.txt", std::ios::out | std::ios::app);
                Reporting::MatlabReporter reporter(out);
                reporter.reportOnTimeStep(d_domain->set, d_domain->nodes);

            }
            bsp_sync();
        }


        size_t total_p = bsp_nprocs();
        size_t s = bsp_pid();
        double *densities = new double[total_p]();
        double current_density = 0;
        bsp_push_reg(densities,total_p * sizeof(double));
        bsp_sync();

        for (auto node : d_domain->nodes)
            current_density += density(d_domain->set, node);

        // send density to each processor
        for (size_t t = 0; t < total_p; t++)
            bsp_put(t, &current_density, densities, s * sizeof(double), sizeof(double));

        bsp_sync();
        // now calculate the total density
        double total_density = 0;
        for (size_t t = 0; t < total_p; t++)
            total_density += densities[t];

        bsp_pop_reg(densities);

        if (s == 0)
            std::cout << "Total density: " << total_density << '\n';

        delete[] densities;
    }

}