#include "Messenger.h"
#include <iostream>

#include "config.h"

Messenger create_messenger(MY_SIZE_T processor, MY_SIZE_T dir)
{
    Messenger m;
    m.d_p = processor;
    m.d_tag[1] = dir;
    m.d_src = -3;
    return m;
}

// Messenger::Messenger(MY_SIZE_T processor, MY_SIZE_T dir)
// :
//     d_p(processor),
//     d_local_idx(0),
//     d_dir(dir)
// {
//     d_tag[0] = dir;
// }

// Messenger::~Messenger()
// {}

// MY_SIZE_T Messenger::processor() const
// {
//     return d_p;
// }

// MY_SIZE_T Messenger::direction() const
// {
//     return d_dir;
// }

// void Messenger::setLocalIdx(MY_SIZE_T idx)
// {
//     d_local_idx = idx;
//     d_tag[1] = idx;
// }

// MY_SIZE_T Messenger::localIdx()
// {
//     return d_local_idx;
// }

// MY_SIZE_T *Messenger::tag()
// {
//     return d_tag;
// }