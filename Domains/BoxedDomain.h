#ifndef INCLUDED_DOMAINS_BOXED_DOMAIN
#define INCLUDED_DOMAINS_BOXED_DOMAIN

#include "DomainInitializer.h"
#include "../VelocitySets/d2q9.h"

namespace Domains {
    class BoxedDomain : public DomainInitializer {

        public:
            BoxedDomain(VelocitySet *set, std::vector<size_t> domainSize, size_t p = 0, size_t totalProcessors = 1);
            ~BoxedDomain();

        protected:
            void connectNodeToNeighbours(size_t idx) override;
            bool isInDomain(std::vector<int> position) override;
            bool isBounceBack(std::vector<int> position);

            size_t processorOfNode(std::vector<int> position) override;
            void sendLocationOfDistribution(Node &node, size_t dir) override;
    };
}

#endif