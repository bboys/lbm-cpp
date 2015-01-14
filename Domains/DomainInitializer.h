#ifndef INCLUDED_DOMAINS_DOMAIN_INITIALIZER
#define INCLUDED_DOMAINS_DOMAIN_INITIALIZER

#include "config.h"

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
            MY_SIZE_T d_p;
            MY_SIZE_T d_total_processors;

            VelocitySet *d_set;
            std::vector<MY_SIZE_T> d_domain_size;

            // hashIdx -> nodeIdx
            std::unordered_map<MY_SIZE_T, MY_SIZE_T> d_map_to_index;
            std::unordered_map<MY_SIZE_T, MY_SIZE_T> d_map_to_messenger;

            std::vector<Node> d_nodes;
            std::vector<std::unique_ptr<BoundaryConditions::PostProcessor>> d_post_processors;

            // parallel:
            std::vector<Messenger> d_messengers;

        public:
            DomainInitializer(VelocitySet *set, std::vector<MY_SIZE_T> domainSize, MY_SIZE_T p = 0, MY_SIZE_T totalProcessors = 1);
            virtual ~DomainInitializer();
            std::unique_ptr<Domain> domain();

        protected:
            void createNodes();
            virtual Node initializeNodeAt(std::vector<int> position);
            // virtual void connectNodeToNeighbours(Node &node);
            virtual void connectNodeToNeighbours(MY_SIZE_T idx);
            virtual bool isInDomain(std::vector<int> position);

            virtual void createPostProcessors(std::vector<Node> &nodes);

            virtual double omega();

            // In order to use a multi dimensional array, we actually
            // convert each vector onto a unique integer
            MY_SIZE_T hashIdxOf(std::vector<int> position, MY_SIZE_T direction = 0);
            MY_SIZE_T idxOf(std::vector<int> position);

            // get the destrination of a distribution after streaming step
            // can be a pointer to a node's distribution, or a pointer to a messenger
            virtual void sendLocationOfDistribution(Node &node, MY_SIZE_T direction);
            virtual MY_SIZE_T processorOfNode(std::vector<int> position);

    };
}

#endif