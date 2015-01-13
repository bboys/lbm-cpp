#ifndef INCLUDED_LBM_MESSENGER
#define INCLUDED_LBM_MESSENGER
#include <iostream>
class Messenger
{
    size_t d_p;
    double d_src;
    double *d_dest;
    size_t d_dir;

    size_t d_local_idx_of_node_on_processor_p;


    public:
        Messenger(size_t processor, size_t dir = 0);
        ~Messenger();

        size_t processor() const;
        size_t direction() const;

        double *source();
        double *dest();
        void dest(double *dest);
        void hoi();
        void setLocalIdx(size_t idx);
        size_t localIdx();


};

#endif