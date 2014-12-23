#ifndef INCLUDED_REPORTING
#define INCLUDED_REPORTING

#include "../main.h"

struct Node;
struct VelocitySet;
// reporting stuff

void reportOnInitialSetup(VelocitySet &set, Node *nodes, size_t dx, size_t dy);
void reportOnNode(VelocitySet &set, Node node, size_t x, size_t y);
void report(VelocitySet &set, Node *nodes, size_t dx, size_t dy);

#endif