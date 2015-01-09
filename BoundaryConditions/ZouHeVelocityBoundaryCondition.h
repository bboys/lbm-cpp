#ifndef INCLUDED_BOUNDARY_CONDITIONS_ZOU_HE_VELOCITY_BOUDNARY
#define INCLUDED_BOUNDARY_CONDITIONS_ZOU_HE_VELOCITY_BOUDNARY

#include "PostProcessor.h"
#include "../LBM/node.h"

namespace BoundaryConditions {
    class ZouHeVelocityBoundary : public PostProcessor
    {
        protected:
            std::vector<Node *> d_acts_on;
            std::vector<double> d_velocity;

        public:
            ZouHeVelocityBoundary(std::vector<double> velocity);
            void add(Node *node);
    };

    class ZouHeVelocityNorthBoundary : public ZouHeVelocityBoundary
    {
        public:
            ZouHeVelocityNorthBoundary(std::vector<double> velocity);
            void process();
    };

    // class ZouHeVelocityEastBoundary : public ZouHeVelocityBoundary
    // {
    //     public:
    //         void process() override;
    // };

    // class ZouHeVelocitySouthBoundary : public ZouHeVelocityBoundary
    // {
    //     public:
    //         void process() override;
    // };

    // class ZouHeVelocityWestBoundary : public ZouHeVelocityBoundary
    // {
    //     public:
    //         void process() override;
    // };

    // Corner
    // class ZouHeVelocityNorthEastCornerBoundary : public ZouHeVelocityBoundary {};
    // class ZouHeVelocityNorthWestCornerBoundary : public ZouHeVelocityBoundary {};

    // class ZouHeVelocitySouthEastCornerBoundary : public ZouHeVelocityBoundary {};
    // class ZouHeVelocitySouthWestCornerBoundary : public ZouHeVelocityBoundary {};
}

#endif