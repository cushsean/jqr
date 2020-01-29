#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "tree.h"
#include "util.h"
#include "list.h"

/*
tree_t* mkTree(int num){
	tree_t* root;
	mkLeaf(root, num--);
	while(num > 0){
		mkLeaf(root, num--);
	}
	return root;
}
*/

tree_b* mkTree_bst(int num){
	srand(time(NULL));
	tree_b* root = NULL;
	for(int i = 0; i < num; i++){
		int rnum = rand()%20;
		void* ptr = &rnum;
		insert_leaf_bst(&root, ptr);
	}
	return root;
}

tree_b* mkLeaf_bst(void* value){
	tree_b* leaf = (tree_b*)malloc(sizeof(tree_b));
	leaf->left = NULL;
	leaf->right = NULL;
	leaf->data = value;
	return leaf;
}

void insert_leaf_bst(tree_b** root, void* value){
	if(*root == NULL){
		*root = mkLeaf_bst(value);
		return;
	}
	int cmp = cmp_int(value, (*root)->data);
	switch(cmp){
		case 1:
			//value > root->data
			return insert_leaf_bst(&(*root)->right, value);
			break;
		case -1:
			//value < root->data
			return insert_leaf_bst(&(*root)->left, value);
			break;
		default:
			//value == root->data
			printf("No leaf created.\n");
			break;
	}
	return;
}

tree_b* find_leaf_bst(tree_b* root, void* value){
	tree_b* leaf = root;
	if(leaf == NULL){
		printf("Leaf not found, returned NULL\n");
		return leaf;
	}
	int cmp = cmp_int(value, leaf->data);
	if(cmp == 1)
		return find_leaf_bst(leaf->right, value);
	else if(cmp == -1)
		return find_leaf_bst(leaf->left, value);
	else{
		return leaf;
	}
}

void rmLeaf_bst(tree_b* root, void* value){
	//case 1: has no children
	//case 2: has 1 child
	//case 3: has 2 children
	
	//get parent && set root to node to be removed
	tree_b* parent = NULL; 
	tree_b* swap = NULL;
	while(root->data != value){
		parent = root;
		if(value < root->data)
			root = root->left;
		else
			root = root->right;
		if(root == NULL){
			/**************************************
				SegFault when value is not in tree
			***************************************/
			printf("Value not found. No leaf removed.\n");
			return;
		}
	}

	if((root->left != NULL) != (root->right != NULL)){
		//1 Child
		if(root->left != NULL)
			swap = root->left;
		else if(root->right != NULL)
			swap = root->right;
	}
	else if(root->left != NULL && root->right != NULL){
		//2 Children
		swap = find_min_bst(root);
	}
	//Any Children
	if(parent != NULL){
		if(parent->data > root->data)
			parent->left = swap;
		else
			parent->right = swap;
	}
	else{

	}
	free(root);
	
	return;
}

//Utils
void ptTree_bst_int(tree_b* root){
	printf("\nPrinting Tree...");
	ptTree_bst_util_int(root, 0);
	return;
}

void ptTree_bst_util_int(tree_b* root, int num){	
	if(root == NULL)
		return;
	
	num += 1;
	
	ptTree_bst_util_int(root->right, num);
	
	printf("\n");
	for(int i=0; i<num; i++)
		printf("\t");
	printf("%d\n", *((int*)root->data));
	
	ptTree_bst_util_int(root->left, num);
	
	return;
}

tree_b* find_min_bst(tree_b* root){
	if(root->left == NULL)
		return root;
	return find_min_bst(root->left);
}

tree_b* find_max_bst(tree_b* root){
	if(root->right == NULL)
		return root;
	return find_max_bst(root->right);
}