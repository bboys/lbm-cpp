#ifndef INCLUDED_DOMAIN_PERIODIC
#define INCLUDED_DOMAIN_PERIODIC

#include "../main.h"

struct Node;
struct VelocitySet;

void initializeNodeAt(VelocitySet &set, Node &node);
void connectNodeToNeighbours(VelocitySet &set, Node *nodes, size_t x, size_t y, size_t dx, size_t dy);
Node *initialize(VelocitySet &set, size_t &totalNodes);

bool periodic(size_t x, size_t y, size_t dx, size_t dy);
bool bounceback(size_t x, size_t y, size_t dx, size_t dy);

#endif