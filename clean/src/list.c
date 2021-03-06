#include "list.h"

struct llist{
	int type;
	int circle;
	size_t size;
	struct node *head;
	struct node *tail;
	void (*datacpy)(void *dest, void *src);
	int (*datacmp)(void *data1, void *data2);
	void (*datafree)(void **data);
};

typedef struct node{
	void *data;
	struct node *next;
	struct node *prev;
}node_t;


// PRIVATE FUNCTIONS


/**
 * Allocates space for a new node and copies the users data into the node using 
 * the user specified datacpy function pointer.
 */
static node_t* node_create(llist *list, void *data, size_t size){
	node_t *node = calloc(1, sizeof(node_t));
	node->data = calloc(1, size);
	list->datacpy(node->data, data);
	return node;
}


/**
 * Swappes the place of two consecutive nodes within a list.
 */
static void node_swap(llist *list, node_t *pr, node_t *n1, 
						node_t *n2, node_t *nx){
	if(n1 == NULL || n2 == NULL){
		fprintf(stderr, "Swap failed.\n");
		fprintf(stderr, "One or more of the passed nodes were NULL.\n");
		return;
	}
	
	// Set next pointers
	
	if(pr != NULL)
		pr->next = n2;
	n2->next = n1;
	n1->next = nx;

	// Set prev pointers

	if(list->type == DOUBLY){
		n2->prev = pr;
		n1->prev = n2;
		if(nx != NULL)
			nx->prev = n1;
	}

	// Reset head and tail
	if(list->head == n1)
		list->head = n2;
	if(list->tail == n2)
		list->tail = n1;
	
	return;
}

static void insert_at_head(llist *list, node_t *new_node){
	new_node->next = list->head;
	if(list->tail == NULL)
		list->tail = new_node;
	if(list->circle == TRUE){
		if(list->type == DOUBLY)
			new_node->prev = list->tail;
		if(list->tail != NULL)
			list->tail->next = new_node;
	}
	if(list->head != NULL && list->type == DOUBLY)
		list->head->prev = new_node;

	list->head = new_node;

	return;
}

static void insert_node(llist *list, node_t *new_node, node_t *PR){
	node_t *NX = NULL;

	if(PR != NULL)
		NX = PR->next;

	new_node->next = NX;
	if(PR != NULL)
		PR->next = new_node;
	if(list->type == DOUBLY){
		new_node->prev = PR;
		if(NX != NULL)
			NX->prev = new_node;
	}

	// Update tail
	if(PR == list->tail && PR != NULL)
		list->tail = new_node;

	return;
}


static int delete_all_nodes(llist *list){
	if(list->tail != NULL)
		list->tail->next = NULL;
	while(list->head != NULL){
		node_t *next_node = list->head->next;

		// Free data and node, setting head to next_node
		list->datafree(&list->head->data);
		free(list->head);
		list->head = next_node;
	}

	// Set head and tail to NULL and size to 0
	list->head = list->tail = NULL;
	list->size = 0;

	// Return Success
	return 0;
}


static void* find_nth(llist *list, int n){
	if(n == -1)
		n = list->size;
	if(n > list->size){
		fprintf(stderr, "n is greater than the number of items in the list.\n");
		return NULL;
	}
	node_t *curr = list->head;
	for(; n > 0; n--)
		curr = curr->next;
	
	if(curr == NULL)
		return NULL;
		
	return curr->data;
}


// PUBLIC FUNCTIONS


llist* llist_create(int type, int circularly, 
						void (*datacpy)(void *dest, void *src), 
						int (*datacmp)(void *data1, void *data2),
						void (*datafree)(void **data)){
	llist* list = calloc(1, sizeof(llist));
	list->head = NULL;
	list->tail = list->head;
	list->type = type;
	list->circle = circularly;
	list->size = 0;
	list->datacpy = datacpy;
	list->datacmp = datacmp;
	list->datafree = datafree;
	
	return list;
}


void llist_insert(llist *list, int nth_mode, void *insert_point, 
					void *data, size_t size){
	node_t *new_node = node_create(list, data, size);
	if(nth_mode == TRUE){
		int n = *(int*)insert_point;
		if(n == -1)
			n = list->size;
		if(n > list->size){
			fprintf(stderr, "The item number specified is greater than the ");
			fprintf(stderr, "size of the list.\n");

				// Free new_node
				list->datafree(&new_node->data);
				free(new_node);
				new_node = NULL;
				
				// Return Failure
				return;
		}
		if(list->head == NULL)
			insert_at_head(list, new_node);
		else{
			node_t *PR = list->head;
			
			for(int i = 0; i < n-1; i++)
				PR = PR->next;

			insert_node(list, new_node, PR);
		}			
	}
	else if(insert_point != NULL){
		node_t *curr = list->head;
		while(list->datacmp(curr->data, insert_point) != 0){
			if(curr == list->tail){
				fprintf(stderr, "There was no instance of \"insert_point\"");
				fprintf(stderr, " found in the linked list.\n");
				
				// Free new_node
				list->datafree(&new_node->data);
				free(new_node);
				new_node = NULL;
				
				// Return Failure
				return;
			}
			curr = curr->next;
		}
		insert_node(list, new_node, curr);
	}
	else{
		// nth_mode == FALSE && insert_point == NULL
		insert_at_head(list, new_node);
	}

	list->size++;
	return;
}


void* llist_find(llist *list, int nth_mode, void *item, size_t instance){
	if(nth_mode == TRUE){
		return find_nth(list, *(int*)item);
	}
	if(list->head == NULL){
		fprintf(stderr, "Head of list does not exist.\n");
		// Return Failure
		return NULL;
	}

	node_t *curr = list->head;
	int i = 0;
	while(1==1){
		if(list->datacmp(curr->data, item) == 0){
			if(++i == instance)
				return curr->data;
		}
		
		curr = curr->next;
		
		if(curr == list->head || curr == NULL){
			fprintf(stderr, "Only found %d instance(s) of \"item\" ", i);
			fprintf(stderr, "in the linked list.\n");
			// Return Failure
			return NULL;
		}
	}
}


void llist_sort(llist *list){
	// Sorts a linked list using Bubble Sort
	node_t *curr = list->head;
	node_t *NX = NULL;
	node_t *curr_pr = curr->prev; // If type is SINGLY curr->prev will be NULL

	for(int i=0; i<list->size-1; i++){
		for(int n=0; n<list->size-1-i; n++){
			NX = curr->next;
			if(list->datacmp(curr->data, NX->data) == 1){
				node_swap(list, curr_pr, curr, NX, NX->next);
				curr_pr = NX;
			}
			else{
				curr_pr = curr;
				curr = curr->next;
			}
		}
		curr = list->head;
		curr_pr = curr->prev; // If type is SINGLY curr->prev will be NULL
	}
	return;
}


int llist_delete(llist *list, int nth_mode, void *data){
	if(list->head == NULL){
		fprintf(stderr, "Head of list does not exist.\n");
		// Return Failure
		return 1;
	}

	// Delete All

	if(data == NULL || list->size == 1){
		return delete_all_nodes(list);
	}
	
	node_t *rm_node = list->head;
	node_t *rm_prev = NULL;
	
	if(nth_mode == TRUE){
		int n = *(int*)data;
		for(; n > 0; n--){
			rm_prev = rm_node;
			rm_node = rm_node->next;
		}
	}
	else{	
		// Find first instance of data
		while(list->datacmp(rm_node->data, data) != 0){
			if(rm_node == list->tail){
				fprintf(stderr, "There was no instance of \"data\"");
				fprintf(stderr, " found in the linked list.\n");
				// Return Failure
				return 1;
			}
			rm_prev = rm_node;
			rm_node = rm_node->next;
		}
	}

	// REMOVE NODE

	if(rm_node == list->head){
		// rm_node is the head node
		list->head = rm_node->next;

		if(list->circle == TRUE){
			// Relink tail
			list->tail->next = list->head;
			if(list->type == DOUBLY)
				list->head->prev = list->tail;
		}
		else
			list->head->prev = NULL;
	}

	else{
		// rm_node is not the head node
		if(rm_prev != NULL)
			rm_prev->next = rm_node->next;
		if(list->type == DOUBLY && rm_node->next != NULL)
			rm_node->next->prev = rm_node->prev;
	}
	
	// Free the data and the node
	list->datafree(&rm_node->data);
	free(rm_node);
	rm_node = NULL;

	// Reduce size of list by 1
	if(--list->size == 0)
		list->head = list->tail = NULL;

	// Return success
	return 0;
}


void llist_destroy(llist **list){

	// If head is not NULL delete entire list
	if((*list)->head != NULL)
		llist_delete(*list, FALSE, NULL);
	
	free(*list);
	*list = NULL;

	return;
}


void llist_print(llist *list, void (*data_print)(void *data)){

	node_t *curr = list->head;
	
	for(int i = 0; i < list->size; i++, curr = curr->next){
		if(i == 0)
			printf("HEAD: ");

		if(i == list->size-1)
			printf("TAIL: ");

		(*data_print)(curr->data);
	
		if(i != list->size-1)
			printf(" -> ");
	}
	printf("\n\n");

	return;
}


size_t llist_size(llist *list){
	return list->size;
}


int llist_cmp(llist *list, void *data1, void *data2){
	return list->datacmp(data1, data2);
}