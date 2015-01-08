// #ifndef INCLUDED_BOUNDARY_CONDITIONS_POST_PROCESSOR
// #define INCLUDED_BOUNDARY_CONDITIONS_POST_PROCESSOR

// #include "../LBM/node.h"

// namespace BoundaryConditions {
//     class PostProcessor
//     {
//         public:
//             virtual void process() = 0;
//     };

//     class ZouHeBoundary : PostProcessor
//     {
//         std::vector<Node *> d_acts_on;

//         public:
//             ZouHeBoundary(std::vector<double> velocity);
//             ZouHeBoundary(double density);
//     };

//     class ZouHeNorthBoundary : PostProcessor
//     {
//         public:
//             void process() override;
//     };

//     class ZouHeEastBoundary : PostProcessor
//     {
//         public:
//             void process() override;
//     };

//     class ZouHeSouthBoundary : PostProcessor
//     {
//         public:
//             void process() override;
//     };

//     class ZouHeWestBoundary : PostProcessor
//     {
//         public:
//             void process() override;
//     };

//     // Corner
//     // class ZouHeNorthEastCornerBoundary : PostProcessor {};
//     // class ZouHeNorthWestCornerBoundary : PostProcessor {};

//     // class ZouHeSouthEastCornerBoundary : PostProcessor {};
//     // class ZouHeSouthWestCornerBoundary : PostProcessor {};
// }
// #endif