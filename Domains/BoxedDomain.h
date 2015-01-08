#ifndef INCLUDED_DOMAINS_BOXED_DOMAIN
#define INCLUDED_DOMAINS_BOXED_DOMAIN

#include "DomainInitializer.h"
#include "../VelocitySets/d2q9.h"

namespace Domains {
    class BoxedDomain : public DomainInitializer {

        public:
            BoxedDomain(VelocitySet &set, std::vector<size_t> domainSize);
            ~BoxedDomain();

        protected:
            void connectNodeToNeighbours(Node &node) override;
            bool isInDomain(std::vector<int> position) override;
            bool isBounceBack(std::vector<int> position);
    };
}

#endif