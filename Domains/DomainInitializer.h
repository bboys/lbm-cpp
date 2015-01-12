#ifndef INCLUDED_DOMAINS_DOMAIN_INITIALIZER
#define INCLUDED_DOMAINS_DOMAIN_INITIALIZER

#include <vector>
#include <unordered_map>
#include <memory>

#include "Domain.h"

#include "../VelocitySets/VelocitySet.h"
#include "../LBM/node.h"
#include "../BoundaryConditions/ZouHe.h"
#include "../BoundaryConditions/PostProcessor.h"

namespace Domains {
    class DomainInitializer {
        protected:
            VelocitySet *d_set;
            std::vector<size_t> d_domain_size;

            // hashIdx -> nodeIdx
            std::unordered_map<size_t, size_t> d_map_to_index;

            std::vector<Node> d_nodes;
            std::vector<std::unique_ptr<BoundaryConditions::PostProcessor>> d_post_processors;

        public:
            DomainInitializer(VelocitySet *set, std::vector<size_t> domainSize);
            virtual ~DomainInitializer();
            std::unique_ptr<Domain> domain();

        protected:
            void createNodes();
            virtual Node initializeNodeAt(std::vector<int> position);
            virtual void connectNodeToNeighbours(Node &node);
            virtual bool isInDomain(std::vector<int> position);

            virtual void createPostProcessors(std::vector<Node> &nodes);

            virtual double omega();

            // In order to use a multi dimensional array, we actually
            // convert each vector onto a unique integer
            size_t hashIdxOf(std::vector<int> position);
            size_t idxOf(std::vector<int> position);

    };
}

#endif