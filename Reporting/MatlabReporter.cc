#include "MatlabReporter.h"
#include "../config.h"

namespace Reporting {

    MatlabReporter::MatlabReporter(std::ostream &ofstream)
    :
        d_ostream(ofstream)
    {};

    void MatlabReporter::initialReport(VelocitySet *set, MY_SIZE_T dx, MY_SIZE_T dy)
    {
        d_ostream << "LBM D" << set->nDimensions << "Q" << set->nDirections << " simulation." << '\n'
            << "Grid size " << dx << "x" << dy
            << '\n' << '\n';
    }

    void MatlabReporter::reportOnTimeStep(VelocitySet *set, Node *nodes, MY_SIZE_T totalNodes)
    {
        for (MY_SIZE_T idx = 0; idx < totalNodes; ++idx)
            showNode(set, nodes[idx]);
    }

    void MatlabReporter::reportOnTimeStep(VelocitySet *set, std::vector<Node> nodes)
    {
        for (auto node : nodes)
            showNode(set, node);
    }

    void MatlabReporter::showVector(double *vector, MY_SIZE_T nDimensions)
    {
        for (MY_SIZE_T dim = 0; dim < (nDimensions - 1); ++dim)
            d_ostream << vector[dim] << ", ";
        d_ostream << vector[nDimensions - 1];
    }

    void MatlabReporter::showVector(int *vector, MY_SIZE_T nDimensions)
    {
        for (MY_SIZE_T dim = 0; dim < (nDimensions - 1); ++dim)
            d_ostream << vector[dim] << ", ";
        d_ostream << vector[nDimensions - 1];
    }

    void MatlabReporter::showVector(size_t *vector, MY_SIZE_T nDimensions)
    {
        for (MY_SIZE_T dim = 0; dim < (nDimensions - 1); ++dim)
            d_ostream << vector[dim] << ", ";
        d_ostream << vector[nDimensions - 1];
    }

    void MatlabReporter::showNode(VelocitySet *set, Node &node)
    {
        MY_SIZE_T nDimensions = set->nDimensions;
        showVector(node.position, nDimensions);
        double node_density = density(set, node);
        d_ostream << ", " << node_density << ", ";
        double *node_velocity = velocity(set, node);
        showVector(node_velocity, nDimensions);
        d_ostream << '\n';
    }
}