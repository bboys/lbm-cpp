#ifndef INCLUDED_VELOCITY_SET_D2Q9
#define INCLUDED_VELOCITY_SET_D2Q9

#include "../config.h"
#include "VelocitySet.h"

// namespace VelocitySets {
    class D2Q9 : public VelocitySet
    {
        public:
            D2Q9();
            ~D2Q9();
            MY_SIZE_T oppositeDirectionOf(MY_SIZE_T dir) override;
    };

    inline MY_SIZE_T D2Q9::oppositeDirectionOf(MY_SIZE_T dir)
    {
        switch (dir)
        {
            case 0: return 0;
            case 1: return 3;
            case 2: return 4;
            case 3: return 1;
            case 4: return 2;
            case 5: return 7;
            case 6: return 8;
            case 7: return 5;
            case 8: return 6;
            default: throw "Invalid direction";
        }
    }
// }

#endif