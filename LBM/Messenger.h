#ifndef INCLUDED_LBM_MESSENGER
#define INCLUDED_LBM_MESSENGER
#include <iostream>

struct Messenger {
    size_t d_p; // the processor that the source needs to be sent to
    size_t d_tag[2]; // the local idx and the direction of the node that needs the source
    double d_src; // the source a node points at
};

Messenger create_messenger(size_t processor, size_t dir);

// class Messenger
// {
//     // the processor to which the message will be send
//     size_t d_p;
//     // the tag, consisting of the local idx and the direction
//     size_t d_tag[2];
//     // the local index of the node on processor d_p
//     size_t d_local_idx;

//     public:
//         size_t d_dir;
//         double d_src = 0;
//         Messenger(size_t processor, size_t dir);
//         ~Messenger();

//         size_t processor() const;
//         size_t direction() const;

//         double *source();

//         void setLocalIdx(size_t idx);
//         size_t localIdx();
//         size_t *tag();

// };

#endif