#include "Messenger.h"
#include <iostream>

Messenger create_messenger(size_t processor, size_t dir)
{
    Messenger m;
    m.d_p = processor;
    m.d_tag[1] = dir;
    m.d_src = -3;
    return m;
}

// Messenger::Messenger(size_t processor, size_t dir)
// :
//     d_p(processor),
//     d_local_idx(0),
//     d_dir(dir)
// {
//     d_tag[0] = dir;
// }

// Messenger::~Messenger()
// {}

// size_t Messenger::processor() const
// {
//     return d_p;
// }

// size_t Messenger::direction() const
// {
//     return d_dir;
// }

// void Messenger::setLocalIdx(size_t idx)
// {
//     d_local_idx = idx;
//     d_tag[1] = idx;
// }

// size_t Messenger::localIdx()
// {
//     return d_local_idx;
// }

// size_t *Messenger::tag()
// {
//     return d_tag;
// }