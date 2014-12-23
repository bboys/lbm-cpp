#ifndef INCLUDED_DOMAIN_PERIODIC
#define INCLUDED_DOMAIN_PERIODIC

#include "../main.h"
#include "../reporting/reporting.h"

struct Node;
struct VelocitySet;

void initializeNodeAt(VelocitySet &set, Node &node, size_t idx);
void connectNodeToNeighbours(VelocitySet &set, Node *nodes, size_t x, size_t y, size_t dx, size_t dy);
Node *initialize(VelocitySet &set, size_t &totalNodes, size_t dx, size_t dy);

bool periodic(size_t x, size_t y, size_t dx, size_t dy);
bool bounceback(size_t x, size_t y, size_t dx, size_t dy);

#endif