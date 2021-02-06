#ifndef __NODE__
#define __NODE__

#include "utils.h"
#define MAX_CHILDREN 4


/**
 * Data structure containing the node information
 */
struct node_s{
    int priority;
    float acc_reward;
    int depth;
    int num_childs;
    move_t move;
    state_t state;
	//childrenScore store the average/maximumm acc_reward(based on the propagation method) of its children (if this node don't have any child, store its own acc_reward)
	float children_score;
    struct node_s* parent;
};

typedef struct node_s node_t;


#endif
