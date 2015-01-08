#ifndef INCLUDED_VELOCITY_SET_D2Q9
#define INCLUDED_VELOCITY_SET_D2Q9

#include "VelocitySet.h"

// namespace VelocitySets {
//     class D2Q9 : VelocitySet
//     {
//         public:
//             D2Q9();
//             ~D2Q9();

//     };
// }

struct VelocitySet;

namespace VelocitySets { namespace D2Q9 {
    void initializeVelocitySet(VelocitySet &set);
    size_t oppositeDirectionOf(size_t dir);
}}

#endif