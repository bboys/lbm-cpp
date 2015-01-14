#ifndef INCLUDED_LBM_MESSENGER
#define INCLUDED_LBM_MESSENGER

#include "config.h"
#include <iostream>

struct Messenger {
    MY_SIZE_T d_p; // the processor that the source needs to be sent to
    MY_SIZE_T d_tag[2]; // the local idx and the direction of the node that needs the source
    double d_src; // the source a node points at
};

Messenger create_messenger(MY_SIZE_T processor, MY_SIZE_T dir);

// class Messenger
// {
//     // the processor to which the message will be send
//     MY_SIZE_T d_p;
//     // the tag, consisting of the local idx and the direction
//     MY_SIZE_T d_tag[2];
//     // the local index of the node on processor d_p
//     MY_SIZE_T d_local_idx;

//     public:
//         MY_SIZE_T d_dir;
//         double d_src = 0;
//         Messenger(MY_SIZE_T processor, MY_SIZE_T dir);
//         ~Messenger();

//         MY_SIZE_T processor() const;
//         MY_SIZE_T direction() const;

//         double *source();

//         void setLocalIdx(MY_SIZE_T idx);
//         MY_SIZE_T localIdx();
//         MY_SIZE_T *tag();

// };

#endif