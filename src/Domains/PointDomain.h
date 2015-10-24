#ifndef INCLUDED_DOMAINS_POINT_DOMAIN
#define INCLUDED_DOMAINS_POINT_DOMAIN

#include "DomainInitializer.h"
#include "../VelocitySets/d2q9.h"

namespace Domains {
    class PointDomain : public DomainInitializer {

        protected:
            Node initializeNodeAt(std::vector<int> &position) override;
            void connectNodeToNeighbours(size_t idx) override;
            bool isInDomain(std::vector<int> &position) override;

            bool isBounceBack(std::vector<int> position);
            double omega() override;
    };
}

#endif