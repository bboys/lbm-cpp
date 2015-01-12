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
#include "../LBM/Messenger.h"

namespace Domains {
    class DomainInitializer {
        protected:
            size_t d_p;
            size_t d_total_processors;

            VelocitySet *d_set;
            std::vector<size_t> d_domain_size;

            // hashIdx -> nodeIdx
            std::unordered_map<size_t, size_t> d_map_to_index;
            std::unordered_map<size_t, size_t> d_map_to_messenger;

            std::vector<Node> d_nodes;
            std::vector<std::unique_ptr<BoundaryConditions::PostProcessor>> d_post_processors;

            // parallel:
            std::vector<Messenger> d_messengers;

        public:
            DomainInitializer(VelocitySet *set, std::vector<size_t> domainSize, size_t p = 0, size_t totalProcessors = 1);
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
            size_t hashIdxOf(std::vector<int> position, size_t direction = 0);
            size_t idxOf(std::vector<int> position);

            // get the destrination of a distribution after streaming step
            // can be a pointer to a node's distribution, or a pointer to a messenger
            virtual void sendLocationOfDistribution(Node &node, size_t direction);
            virtual double *destination(std::vector<int> position, size_t direction);
            virtual size_t processorOfNode(std::vector<int> position);

    };
}

#endif