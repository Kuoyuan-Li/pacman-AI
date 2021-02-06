#ifndef __AI__
#define __AI__

#include <stdint.h>
#include <unistd.h>
#include "node.h"
#include "priority_queue.h"
#define explored_initial_size  1

typedef struct explored_nodes_s explored_nodes_t;

void initialize_ai();

move_t get_next_move( state_t init_state, int budget, propagation_t propagation, char* stats );

#endif
