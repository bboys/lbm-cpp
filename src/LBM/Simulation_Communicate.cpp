#include "Simulation.h"
#include "node.h"
#include <memory>

#include "../LBM/parallel_bsp.h"

namespace LBM {

    // The communication and stream step use a push method
    void Simulation::communicate(std::vector<Messenger> messengers)
    {
        // If a node's neighbor is not present on the current processor (P_n) then we
        // will want to send the distribution that processor P_n needs
        //
        // each messenger has a double d_src which contains the value of the distribution
        // that needs to be send to P_n
        // This distribution has been set during the streaming phase
        //
        // We will sent both the distribution (d_src) and a tag that contains the local index of
        // the node and the index of the direction from which the distribution is coming from

        // First we will send the distribution of each messenger to the corresponding processor
        // we also send the node index and the direction index to the processor by using d_tag
        for (auto messenger : messengers)
            bsp_send(messenger.d_p, messenger.d_tag, &messenger.d_src, sizeof(double));
        bsp_sync();

        // Next we retrieve the distributions that have been send to this processor
        // first we determine the target of the distribution (which is stored in the size_t target[2])
        // next we move the distribution to this target's next value
        MCBSP_NUMMSG_TYPE nmessages = 0;
        MCBSP_BYTESIZE_TYPE nbytes = 0;
        bsp_qsize(&nmessages, &nbytes);
        for (MCBSP_NUMMSG_TYPE n = 0; n < nmessages; ++n)
        {
            size_t target[2];
            MCBSP_BYTESIZE_TYPE status;
            bsp_get_tag(&status,&target); // i[0] = idx, i[1] = dir
            if (status > 0)
            {
                double distribution = 0;
                bsp_move(&distribution, sizeof(double));
                d_domain->nodes[target[0]].distributions[target[1]].nextValue = distribution;
            }
        }
    }

}