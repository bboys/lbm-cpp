#ifndef INCLUDED_REPORTING
#define INCLUDED_REPORTING

#include "../main.h"

struct Node;
struct VelocitySet;

void reportOnInitialSetup(VelocitySet &set, Node *nodes, size_t dx, size_t dy);
void reportOnNode(VelocitySet &set, Node node, size_t x, size_t y);
void report(VelocitySet &set, Node *nodes, size_t dx, size_t dy);
void report(VelocitySet &set, Node *nodes, size_t totalNodes);

#endif