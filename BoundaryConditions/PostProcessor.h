#ifndef INCLUDED_BOUNDARY_CONDITIONS_POST_PROCESSOR
#define INCLUDED_BOUNDARY_CONDITIONS_POST_PROCESSOR

#include "../LBM/node.h"

namespace BoundaryConditions {
    class PostProcessor
    {
        public:
            virtual ~PostProcessor();
            virtual void process() = 0;
            virtual void add(Node *) = 0;
    };
}
#endif