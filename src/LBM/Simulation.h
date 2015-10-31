#ifndef INCLUDED_LBM_SIMULATION
#define INCLUDED_LBM_SIMULATION

#include <memory>

#include "../Domains/DomainInitializer.h"
#include "../VelocitySets/VelocitySet.h"
#include "node.h"
#include "../LBM/Messenger.h"

#include "../Reporting/reporting.h"
#include "../Reporting/MatlabReporter.h"

namespace LBM {
    class Simulation
    {
        using Initializer_Ptr = std::unique_ptr<Domains::DomainInitializer>;

        std::unique_ptr<Domain> d_domain;

        public:
            Simulation(Initializer_Ptr initializer);
            Simulation(std::unique_ptr<Domain> domain);
            ~Simulation();

            void step();
            void report(::Reporting::MatlabReporter reporter);
            void report();

        private:
            void stream(VelocitySet *set, std::vector<Node> &nodes);
            void postStreamProcess();
            void collission(VelocitySet *set, std::vector<Node> &nodes);
            void communicate(std::vector<Messenger> messengers);

    };
}
#endif