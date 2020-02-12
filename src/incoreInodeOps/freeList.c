#include <stdlib.h>
#include "incoreInodeOps/freeList.h"

/* This is doubly-linked, non-circular linked list. */
static Node* freeList;

void initFreeInCoreINodeList() {
    freeList = NULL;
    int i;
    for(i = 0; i < INODE_BUFFER_SIZE; i++) {
        Node* node = (Node*)malloc(sizeof(struct Node));
        node->inode = (inCoreiNode*)malloc(sizeof(inCoreiNode));

		freeListInsert(node);
    }
}

void freeListInsert(Node* node) {
	node->next = freeList;
	node->prev = NULL;
	if (freeList != NULL) {
		freeList->prev = node;
	}
	freeList = node;
}

void freeListRemove(Node* node) {
    Node* workingNode = freeList;
    while(NULL != workingNode) {
		if (node == workingNode) {
			if (node->next != NULL) {
				node->next->prev = node->prev;
			}
			if (node->prev != NULL) {
				node->prev->next = node->next;
			}
			node->next = NULL;
			node->prev = NULL;
			break;
		}
		workingNode = workingNode->next;
	}
}

extern Node* getFreeINodeFromList() {
    Node* head = freeList;
    return head;
}

extern bool checkFreeListEmpty() {
    if (freeList == NULL)
        return true;
    return false;
}

