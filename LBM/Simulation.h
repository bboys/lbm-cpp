#ifndef INCLUDED_LBM_SIMULATION
#define INCLUDED_LBM_SIMULATION

#include "../Domains/DomainInitializer.h"
#include "../VelocitySets/VelocitySet.h"
#include "node.h"

namespace LBM {
    class Simulation
    {
        std::unique_ptr<Domain> d_domain;

        public:
            Simulation(Domains::DomainInitializer *initializer);
            ~Simulation();
            void run();

        private:
            void stream(VelocitySet *set, std::vector<Node> &nodes);
            void postStreamProcess();
            void collission(VelocitySet *set, std::vector<Node> &nodes);
            void report();

    };
}
#endif