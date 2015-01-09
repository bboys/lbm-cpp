#include "MatlabReporter.h"

namespace Reporting {

    MatlabReporter::MatlabReporter(std::ostream &ofstream)
    :
        d_ostream(ofstream)
    {};

    void MatlabReporter::initialReport(VelocitySet *set, size_t dx, size_t dy)
    {
        d_ostream << "LBM D" << set->nDimensions << "Q" << set->nDirections << " simulation." << '\n'
            << "Grid size " << dx << "x" << dy
            << '\n' << '\n';
    }

    void MatlabReporter::reportOnTimeStep(VelocitySet *set, Node *nodes, size_t totalNodes)
    {
        for (size_t idx = 0; idx < totalNodes; ++idx)
            showNode(set, nodes[idx]);
    }

    void MatlabReporter::showVector(double *vector, size_t nDimensions)
    {
        for (size_t dim = 0; dim < (nDimensions - 1); ++dim)
            d_ostream << vector[dim] << ", ";
        d_ostream << vector[nDimensions - 1];
    }

    void MatlabReporter::showVector(int *vector, size_t nDimensions)
    {
        for (size_t dim = 0; dim < (nDimensions - 1); ++dim)
            d_ostream << vector[dim] << ", ";
        d_ostream << vector[nDimensions - 1];
    }

    void MatlabReporter::showVector(size_t *vector, size_t nDimensions)
    {
        for (size_t dim = 0; dim < (nDimensions - 1); ++dim)
            d_ostream << vector[dim] << ", ";
        d_ostream << vector[nDimensions - 1];
    }

    void MatlabReporter::showNode(VelocitySet *set, Node &node)
    {
        size_t nDimensions = set->nDimensions;
        showVector(node.position, nDimensions);
        double node_density = density(set, node);
        d_ostream << ", " << node_density << ", ";
        double *node_velocity = velocity(set, node);
        showVector(node_velocity, nDimensions);
        d_ostream << '\n';
    }
}