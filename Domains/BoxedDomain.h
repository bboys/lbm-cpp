#ifndef INCLUDED_DOMAINS_BOXED_DOMAIN
#define INCLUDED_DOMAINS_BOXED_DOMAIN

#include "config.h"

#include "DomainInitializer.h"
#include "../VelocitySets/d2q9.h"

namespace Domains {
    class BoxedDomain : public DomainInitializer {

        public:
            BoxedDomain(VelocitySet *set, std::vector<MY_SIZE_T> domainSize, MY_SIZE_T p = 0, MY_SIZE_T totalProcessors = 1);
            ~BoxedDomain();

        protected:
            void connectNodeToNeighbours(MY_SIZE_T idx) override;
            bool isInDomain(std::vector<int> position) override;
            bool isBounceBack(std::vector<int> position);

            MY_SIZE_T processorOfNode(std::vector<int> position) override;
            void sendLocationOfDistribution(Node &node, MY_SIZE_T dir) override;
    };
}

#endif