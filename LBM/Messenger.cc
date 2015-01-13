#include "Messenger.h"
#include <iostream>

Messenger::Messenger(size_t processor, size_t dir)
:
    d_p(processor),
    d_dir(dir),
    d_local_idx_of_node_on_processor_p(0)
{}

Messenger::~Messenger()
{}

size_t Messenger::processor() const
{
    return d_p;
}

size_t Messenger::direction() const
{
    return d_dir;
}

double *Messenger::source()
{
    return &d_src;
}
double *Messenger::dest()
{
    return d_dest;
}
void Messenger::dest(double *dest)
{
    d_dest = dest;
}

void Messenger::setLocalIdx(size_t idx)
{
    d_local_idx_of_node_on_processor_p = idx;
}

size_t Messenger::localIdx()
{
    return d_local_idx_of_node_on_processor_p;
}
