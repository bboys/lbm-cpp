#ifndef INCLUDED_REPORTING_MATLAB_REPORTER
#define INCLUDED_REPORTING_MATLAB_REPORTER

#include "../config.h"

#include <iostream>
#include <vector>

#include <string>
#include "../VelocitySets/VelocitySet.h"
#include "../LBM/node.h"

namespace Reporting {
    class MatlabReporter {
        std::ostream &d_ostream;

        public:
        MatlabReporter(std::ostream &ofstream);

        void initialReport(VelocitySet *set, MY_SIZE_T dx, MY_SIZE_T dy);
        void reportOnTimeStep(VelocitySet *set, Node *nodes, MY_SIZE_T totalNodes);
        void reportOnTimeStep(VelocitySet *set, std::vector<Node> nodes);

        private:
        void showVector(double *vector, MY_SIZE_T nDimensions);
        void showVector(int *vector, MY_SIZE_T nDimensions);
        void showVector(size_t *vector, MY_SIZE_T nDimensions);
        void showNode(VelocitySet *set, Node &node);

    };
}

#endif