#ifndef INCLUDED_DOMAINS_LID_DRIVEN_CAVITY
#define INCLUDED_DOMAINS_LID_DRIVEN_CAVITY

#include "DomainInitializer.h"
#include "../VelocitySets/d2q9.h"

namespace Domains {
    class LidDrivenCavityDomain : public DomainInitializer {

        public:
            LidDrivenCavityDomain(VelocitySet *set, std::vector<size_t> domainSize);
            ~LidDrivenCavityDomain();

        protected:
            void connectNodeToNeighbours(Node &node) override;

            bool isInDomain(std::vector<int> position) override;
            bool isBounceBack(std::vector<int> position);
            bool isZouHe(std::vector<int> position);
            bool pointsOutwards(std::vector<int> position);

            void createPostProcessors(std::vector<Node> &nodes) override;
    };
}

#endif