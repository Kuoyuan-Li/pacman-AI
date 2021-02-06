
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>


#include "ai.h"
#include "utils.h"
#include "priority_queue.h"
#include "pacman.h"

#define BECOME_INVINCIBLE 10
#define LOSS_LIFE 10
#define GAME_OVER 100
#define MAX_ACTION_NUM 4
#define TWO 2
struct heap h;

float get_reward( node_t* n );
void propagateBackScoreToFirstAction(node_t* new_node,node_t* root,propagation_t propagation);
/**
 * Function called by pacman.c
*/
void initialize_ai(){
	heap_init(&h);
}

/**
 * function to copy a src into a dst state
*/
void copy_state(state_t* dst, state_t* src){
	//Location of Ghosts and Pacman
	memcpy( dst->Loc, src->Loc, 5*2*sizeof(int) );

    //Direction of Ghosts and Pacman
	memcpy( dst->Dir, src->Dir, 5*2*sizeof(int) );

    //Default location in case Pacman/Ghosts die
	memcpy( dst->StartingPoints, src->StartingPoints, 5*2*sizeof(int) );

    //Check for invincibility
    dst->Invincible = src->Invincible;
    
    //Number of pellets left in level
    dst->Food = src->Food;
    
    //Main level array
	memcpy( dst->Level, src->Level, 29*28*sizeof(int) );

    //What level number are we on?
    dst->LevelNumber = src->LevelNumber;
    
    //Keep track of how many points to give for eating ghosts
    dst->GhostsInARow = src->GhostsInARow;

    //How long left for invincibility
    dst->tleft = src->tleft;

    //Initial points
    dst->Points = src->Points;

    //Remiaining Lives
    dst->Lives = src->Lives;   

}

node_t* create_init_node( state_t* init_state ){
	node_t * new_n = (node_t *) malloc(sizeof(node_t));
	new_n->parent = NULL;
	new_n->priority = 0;
	new_n->depth = 0;
	new_n->num_childs = 0;
	copy_state(&(new_n->state), init_state);
	new_n->acc_reward =  get_reward( new_n );
	new_n->children_score=new_n->acc_reward;
	return new_n;
	
}


float heuristic( node_t* n ){
	float h = 0;
	//FILL IN MISSING CODE
	//h(n) = i-l-g : i=10 if eaten a fruit and become invincible , l=10 if life already lost; g = 100 if game is over
	int i=0,l=0,g=0;
	if(n->parent != NULL){
		if(n->state.Invincible == 1 && n->parent->state.Invincible == 0){i=BECOME_INVINCIBLE;}
		if(n->state.Lives < n->parent->state.Lives){l=LOSS_LIFE;}
	}
	if(n->state.Lives < 0 ){g=GAME_OVER;}
	h = i-l-g;
	return h;
}

float get_reward ( node_t* n ){
	float reward = 0;
	//FILL IN MISSING CODE
	float node_heuristic = heuristic(n);
	if(n->parent != NULL){
		reward = node_heuristic + n->state.Points - n->parent->state.Points;
	}
	float discount = pow(0.99,n->depth);
	return discount * reward;
}

/**
 * Apply an action to node n and return a new node resulting from executing the action
*/
bool applyAction(node_t* n, node_t** new_node, move_t action ){// if action is facing to the wall,deny this action

	bool changed_dir = false;
	
    //FILL IN MISSING CODE
	//state-points is current score
	copy_state(&((*new_node)->state),&(n->state));
	changed_dir = execute_move_t(&((*new_node)->state), action);
	(*new_node)->parent = n;
	(*new_node)->depth = n->depth+1;
	(*new_node)->priority = -1*((*new_node)->depth);
	(*new_node)->num_childs=0;
	(*new_node)->move=action;
	float new_node_reward = get_reward(*new_node);
	(*new_node)-> acc_reward = new_node_reward + n->acc_reward;
	(*new_node)->children_score=(*new_node)-> acc_reward;
	return changed_dir;

}


/**
 * Find best action by building all possible paths up to budget
 * and back propagate using either max or avg
 */

move_t get_next_move( state_t init_state, int budget, propagation_t propagation, char* stats ){
	
	
	move_t best_action = rand() % 4;

	float best_action_score[4];
	for(unsigned i = 0; i < 4; i++)
	    best_action_score[i] = INT_MIN;

	unsigned generated_nodes = 0; //num of nodes that been generated (explored + the children of explored)
	unsigned expanded_nodes = 0; //num of nodes been explored
	unsigned max_depth = 0; 
	
	//create and initialise explored array
	node_t** explored_nodes_arr = (node_t**)malloc(budget * sizeof(node_t*));
	for(int i=0;i<budget;i++){
		 explored_nodes_arr[i] = NULL;
	}
	int explored_count=0;
	
	int action_list[] = {left,right,up,down};
	
	//Add the initial node
	node_t* root = create_init_node(&init_state);
	//Use the max heap API provided in priority_queue.h
	heap_push(&h,root);
	/*FILL IN THE GRAPH ALGORITHM*/
	node_t* expanding_node = NULL; 
	while(h.count > 0){
		
		if(explored_count >= budget){
			break;
		}
		//deQ and explore this node
		expanding_node = heap_delete(&h);
		*(explored_nodes_arr+explored_count) = expanding_node;
		explored_count++;
		//if all budget is consumed, stop exploring and empty the PQ
		if(explored_count <= budget){
			expanded_nodes ++;
			for(int i=0;i<MAX_ACTION_NUM;i++){
				 //malloc a new node (attached to current explored node) and assert the new node
				node_t* new_node = (node_t *) malloc(sizeof(node_t));
				if(new_node ==NULL){
					exit(EXIT_FAILURE);
				}
				//check if action_list[i] is applicable for current_node ->state, if yes, continue , if not, destroy (free)
				if(applyAction(expanding_node,&new_node,action_list[i])){
					new_node->parent->num_childs = new_node->parent->num_childs+1;
					//propagate back score to first action
					propagateBackScoreToFirstAction(new_node,root,propagation);
					//update the best score of firstmove
					node_t* to_find_firstmove = new_node;
					while(to_find_firstmove->parent != root){
						to_find_firstmove =to_find_firstmove -> parent;
					}
					best_action_score[to_find_firstmove->move] =to_find_firstmove->children_score;
					//check if the new node will lose life, if yes, destroy
					if(new_node->state.Lives < expanding_node->state.Lives){
						expanding_node->num_childs =  expanding_node->num_childs-1;
						free(new_node);
					}else{
						if(max_depth < new_node->depth){
							max_depth =  new_node->depth;
						}
						//add the new node to the heap
						generated_nodes++;
						heap_push(&h,new_node);
					}
				}else{
					//action not applicable, destroy new node
					free(new_node);
				}
			}
		}else{
			emptyPQ(&h);
		}
	}
	//find the best action
	for(int q = 0; q < MAX_ACTION_NUM; q++){
		if(best_action_score[q] > best_action_score[best_action]){
			best_action = action_list[q];
		}else if (best_action_score[q] == best_action_score[best_action]){
			//breaking ties randomly
			int random = rand() % TWO;
			if(random == 0){
				best_action = action_list[q];
			}
		}
	}
	//prepare for output file
	total_generated_nodes+=generated_nodes;
	total_expanded_nodes+=expanded_nodes;
	if(total_max_depth<max_depth){
		total_max_depth = max_depth;
	}
	sprintf(stats, "Max Depth: %d Expanded nodes: %d  Generated nodes: %d\n",max_depth,expanded_nodes,generated_nodes);
	
	if(best_action == left)
		sprintf(stats, "%sSelected action: Left\n",stats);
	if(best_action == right)
		sprintf(stats, "%sSelected action: Right\n",stats);
	if(best_action == up)
		sprintf(stats, "%sSelected action: Up\n",stats);
	if(best_action == down)
		sprintf(stats, "%sSelected action: Down\n",stats);

	sprintf(stats, "%sScore Left %f Right %f Up %f Down %f",stats,best_action_score[left],best_action_score[right],best_action_score[up],best_action_score[down]);
	
	
	//free explored
	for(int i=0;i<budget;i++){
		free(explored_nodes_arr[i]);
		explored_nodes_arr[i] = NULL;
	}
	emptyPQ(&h);
	free(explored_nodes_arr);
	return best_action;
}

void propagateBackScoreToFirstAction(node_t* new_node,node_t* root,propagation_t propagation){
	node_t* tmp_node = new_node;
	node_t* parent_node = tmp_node->parent;
	float prev_parent_child_score=0;
	float prev_parent_child_score_buffer=0;
	while(parent_node != root){
		//max propagation: compare the children_score of node and its parent, if node->children_score > parent_node->children_score, update parent's children_score
		if(propagation == max){
			if(tmp_node->children_score > parent_node->children_score){
				parent_node->children_score = tmp_node->children_score;
			}
		}//avg propagation: calculate the current average for each parent node until the first move
		else if(propagation == avg){
			if(tmp_node ->num_childs == 0){
				prev_parent_child_score = parent_node->children_score;
				parent_node->children_score = (parent_node->children_score *  (parent_node->num_childs-1) + tmp_node -> children_score)/(parent_node->num_childs);
			}else{
				prev_parent_child_score_buffer = parent_node->children_score;
				parent_node->children_score = (parent_node->children_score *  parent_node->num_childs -prev_parent_child_score+ tmp_node -> children_score)/(parent_node->num_childs);
				prev_parent_child_score = prev_parent_child_score_buffer;
			}
		}
		//proceed to next depth(traverse back to root)
		tmp_node =parent_node;
		parent_node = tmp_node->parent;
	}
}
