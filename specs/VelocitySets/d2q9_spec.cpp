#include <bandit/bandit.h>

using namespace bandit;

#include <vector>
#include "../src/VelocitySets/d2q9.h"

go_bandit([](){
    describe("The D2Q9 velocity set", [&](){

        it("Has 9 directions", [&](){
            D2Q9 set;
            AssertThat(set.nDirections, Equals(9));
        });

        it("Can be used to simulate 2D models", [&](){
            D2Q9 set;
            AssertThat(set.nDimensions, Equals(2));
        });

        it("Each direction has an opposite direction", [&](){
            D2Q9 set;

            for (int idx = 0; idx < set.nDirections; ++idx)
            {
                auto direction = set.direction(idx);
                auto neighbour = set.direction(set.oppositeDirectionOf(idx));

                for (int dim = 0; dim < set.nDimensions; ++dim)
                {
                    AssertThat(direction[dim] + neighbour[dim], Equals(0));
                }
            }
        });

        it("it satisfies the first lattice constraint", [&]() {
            D2Q9 set;
            double sum_of_weights = 0;
            for (int dir = 0; dir < set.nDirections; ++dir)
            {
                sum_of_weights += set.weight(dir);
            }
            AssertThat(sum_of_weights, EqualsWithDelta(1, 0.00000000000001));
        });

        it("it satisfies the second lattice constraint", [&]() {
            D2Q9 set;
            std::vector<double> sum = {0.0, 0.0};

            for (int dir = 0; dir < set.nDirections; ++dir)
            {
                auto direction = set.direction(dir);
                sum[0] += set.weight(dir) * direction[0];
                sum[1] += set.weight(dir) * direction[1];
            }

            AssertThat(sum[0], EqualsWithDelta(0, 0.00000000000001));
            AssertThat(sum[1], EqualsWithDelta(0, 0.00000000000001));
        });

        it("it satisfies the third lattice constraint", [&]() {
            D2Q9 set;
            std::vector<double> sum = {0.0, 0.0, 0.0};

            for (int dir = 0; dir < set.nDirections; ++dir)
            {
                auto direction = set.direction(dir);
                sum[0] += set.weight(dir) * direction[0] * direction[0];
                sum[1] += set.weight(dir) * direction[1] * direction[0];
                sum[2] += set.weight(dir) * direction[1] * direction[1];
            }

            auto speed = set.speedOfSoundSquared();
            AssertThat(sum[0], EqualsWithDelta(speed, 0.00000000000001));
            AssertThat(sum[1], EqualsWithDelta(0, 0.00000000000001));
            AssertThat(sum[2], EqualsWithDelta(speed, 0.00000000000001));
        });

        it("it satisfies the fourth lattice constraint", [&]() {
            D2Q9 set;
            std::vector<double> sum = {0.0, 0.0, 0.0, 0.0};

            for (int dir = 0; dir < set.nDirections; ++dir)
            {
                auto direction = set.direction(dir);
                sum[0] += set.weight(dir) * direction[0] * direction[0] * direction[0];
                sum[1] += set.weight(dir) * direction[1] * direction[0] * direction[0];
                sum[2] += set.weight(dir) * direction[1] * direction[1] * direction[0];
                sum[3] += set.weight(dir) * direction[1] * direction[1] * direction[1];
            }

            auto speed = set.speedOfSoundSquared();
            AssertThat(sum[0], EqualsWithDelta(0, 0.00000000000001));
            AssertThat(sum[1], EqualsWithDelta(0, 0.00000000000001));
            AssertThat(sum[2], EqualsWithDelta(0, 0.00000000000001));
            AssertThat(sum[3], EqualsWithDelta(0, 0.00000000000001));
        });


        it("it satisfies the fifth lattice constraint", [&]() {
            D2Q9 set;
            std::vector<double> sum = {0.0, 0.0, 0.0, 0.0, 0.0};

            for (int dir = 0; dir < set.nDirections; ++dir)
            {
                auto direction = set.direction(dir);
                sum[0] += set.weight(dir) * direction[0] * direction[0] * direction[0] * direction[0];
                sum[1] += set.weight(dir) * direction[1] * direction[0] * direction[0] * direction[0];
                sum[2] += set.weight(dir) * direction[1] * direction[1] * direction[0] * direction[0];
                sum[3] += set.weight(dir) * direction[1] * direction[1] * direction[1] * direction[0];
                sum[4] += set.weight(dir) * direction[1] * direction[1] * direction[1] * direction[1];
            }

            auto speed = set.speedOfSoundSquared();
            AssertThat(sum[0], EqualsWithDelta(3 * speed * speed, 0.00000000000001));
            AssertThat(sum[1], EqualsWithDelta(0 * speed * speed, 0.00000000000001));

            AssertThat(sum[2], EqualsWithDelta(1 * speed * speed, 0.00000000000001));

            AssertThat(sum[3], EqualsWithDelta(0 * speed * speed, 0.00000000000001));
            AssertThat(sum[4], EqualsWithDelta(3 * speed * speed, 0.00000000000001));
        });
    });

    it("it satisfies the sixth lattice constraint", [&]() {
        D2Q9 set;
        std::vector<double> sum = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

        for (int dir = 0; dir < set.nDirections; ++dir)
        {
            auto direction = set.direction(dir);
            sum[0] += set.weight(dir) * direction[0] * direction[0] * direction[0] * direction[0] * direction[0];
            sum[1] += set.weight(dir) * direction[1] * direction[0] * direction[0] * direction[0] * direction[0];
            sum[2] += set.weight(dir) * direction[1] * direction[1] * direction[0] * direction[0] * direction[0];
            sum[3] += set.weight(dir) * direction[1] * direction[1] * direction[1] * direction[0] * direction[0];
            sum[4] += set.weight(dir) * direction[1] * direction[1] * direction[1] * direction[1] * direction[0];
            sum[5] += set.weight(dir) * direction[1] * direction[1] * direction[1] * direction[1] * direction[0];
        }

        AssertThat(sum[0], EqualsWithDelta(0, 0.00000000000001));
        AssertThat(sum[1], EqualsWithDelta(0, 0.00000000000001));
        AssertThat(sum[2], EqualsWithDelta(0, 0.00000000000001));
        AssertThat(sum[3], EqualsWithDelta(0, 0.00000000000001));
        AssertThat(sum[4], EqualsWithDelta(0, 0.00000000000001));
        AssertThat(sum[5], EqualsWithDelta(0, 0.00000000000001));
    });
});