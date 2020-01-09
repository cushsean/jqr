#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct node{
	int data;
	struct node *next;
	struct node *prev;
}node_t;

typedef node_t* (*func_t)(node_t*);

node_t* mkNode(int);
node_t* mkList(int, int);
void ptList(node_t*);
node_t* find_node(node_t*, int);
node_t* sort_node(node_t*);
node_t* rmNode(node_t*, int, int);
node_t* insert_at_head(node_t*, node_t*, node_t*);
void insert_node(node_t*, int, int);
void rm_all_nodes(node_t*);
node_t* get_tail(node_t*);


int main(){
	func_t sort_ptr = &sort_node;
	node_t* list;
	list = mkList(10, 0); 
	printf("\t%d\n\n", find_node(list, 3)->data);
	list = rmNode(list, 5, 0);
	ptList(list);
	insert_node(list, 0, 18);
	ptList(list);
	list = (*sort_ptr)(list);
	ptList(list);
	rm_all_nodes(list);
	
	return 0;	
}

//3.3.4.a Create d_list with n number of items
node_t* mkNode(int num){
	node_t* fn_node = (node_t*)malloc(sizeof(node_t));
	fn_node->data = num;
	fn_node->next = NULL;
	fn_node->prev = NULL;
	return fn_node;
}

node_t* mkList(int num, int circle){
	node_t* head = NULL;
	node_t* tail = NULL;
	for (int i=0; i<num; i++){
		head = insert_at_head(head, tail, mkNode(i));
		if(tail == NULL && circle)
			tail = head;
	}
	
	ptList(head);
	return head;
}

//3.3.4.b Navigate through a list
void ptList(node_t *head){
	node_t* tmp = head;
	printf("Printing List...\n");
	do{
		printf("\t%d\n", tmp->data);
		tmp = tmp->next;
	}while(tmp != head && tmp != NULL);
	printf("\n");
	return;	
}

//3.3.4.c Find the first occurance in d_list
node_t* find_node(node_t *head, int value){
	node_t* tmp = head;
	if (tmp != NULL)
		do{
			if(tmp->data == value)
				return tmp;
			tmp = tmp->next;
		}while(tmp != head && tmp != NULL);
	printf("ERROR: FOUND NULL; NO ACTION.\n");
	return NULL;	
}

//3.3.4.d Sort a d_list alphanumerically with fn_ptr
node_t* sort_node(node_t *head){
	node_t *curr = head;
	node_t *last = get_tail(head);
	node_t *NX = NULL;
	int swapped;
		
	//reassign curr to head
	curr = head;
	
	//sort
	do{
		swapped = 0;
		while(curr->next != last){
			NX = curr->next;
			if(curr->data > NX->data){
				if(curr->prev != NULL)
					curr->prev->next = NX;
				NX->prev = curr->prev;
				if(NX->next != NULL)
					NX->next->prev = curr;
				curr->next = NX->next;
				NX->next = curr;
				curr->prev = NX;
				if(head == curr)
					head = curr->prev;
				swapped = 1;
			}
			else
				curr = curr->next;
		}
		last = curr;
		curr = head;
	}while(swapped);
	return head;	
}

//3.3.4.e Removing selected items from d_list
node_t* rmNode(node_t *head, int value, int all){
	//Removes node with data equal to value
	//Non-zero all will remove all nodes with value
	node_t* tail = get_tail(head);
	if(tail == head){
		rm_all_nodes(head);
		return NULL;
	}
	node_t* rm_node = find_node(head, value);
	if(rm_node != NULL){
		do{
			if(rm_node == head){
				head = rm_node->next;
				if(tail->next != NULL){
					tail->next = head;
					head->prev = tail;
				}
				else
					head->prev = NULL;
			}
			else
				rm_node->prev->next = rm_node->next;
			if(rm_node->next != NULL)
				rm_node->next->prev = rm_node->prev;
			if(rm_node == tail)
				break;
			free(rm_node);
		}while(rm_node != tail && all);
	}
	else
		printf("ERROR: Value not found, no node removed.\n");
	return head;
}

//3.3.4.f Insert node at secified location
node_t* insert_at_head(node_t* head, node_t* tail, node_t* insert){
	insert->next = head;
	insert->prev = tail;
	if(head != NULL)
		head->prev = insert;
	if(tail != NULL)
		tail->next = insert;
	return insert;
}

void insert_node(node_t *list, int after, int value){
	node_t* insert_after = find_node(list, after);
	if(insert_after != NULL){
		node_t* new_node = mkNode(value);
		new_node->next = insert_after->next;
		if(insert_after->next != NULL)
			insert_after->next->prev = new_node;
		insert_after->next = new_node;
		new_node->prev = insert_after;
	}
	return;
}

//3.3.4.g-h Destroy d_list
void rm_all_nodes(node_t *head){
	node_t* tail = get_tail(head);
	if(head == NULL)
		printf("ERROR: Passed Pointer is NULL, NO ACTION\n");

	do{
		node_t* tmp = head->next;
		free(head);
		head = tmp;
	}while(head != NULL && head != tail);
	printf("ALL NODES REMOVED. LIST IS DESTROYED!\n");
		
	return;
}

node_t* get_tail(node_t* head){
	node_t* curr = head;
	if(curr->next == NULL)
		goto TERM;
	do{
		curr = curr->next;
	}while(curr->next != NULL && curr->next != head);
	TERM: return curr;
}