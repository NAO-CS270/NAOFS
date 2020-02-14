#include "incoreInodeOps/freeList.h"

#include <string.h>
#include <stdlib.h>

/* This is a doubly-linked circular linked list. */
static Node* freeList;

void printFreeList() {
	int counter = 0;
	Node *workingNode = freeList;

	while (workingNode != NULL) {
		workingNode = workingNode->next;
		if (workingNode == freeList || counter == 40) {
			break;
		}
		counter++;
	}
	printf("Free List Size - %ld\n", counter);
}

void initFreeInCoreINodeList() {
    freeList = NULL;
    int counter;
    for(counter = 0; counter < INODE_BUFFER_SIZE; counter++) {
        Node* node = (Node*)malloc(sizeof(Node));
		memset(node, 0, sizeof(Node));

        node->inode = (inCoreiNode*)malloc(sizeof(inCoreiNode));
		memset(node->inode, 0, sizeof(inCoreiNode));
		node->hash_next = NULL;
		node->hash_prev = NULL;

		freeListInsert(node);
    }
}

void freeListInsert(Node* node) {
	if (freeList == NULL) {
		freeList = node;
		node->next = freeList;
		node->prev = freeList;
		return ;
	}
	node->next = freeList;
	node->prev = freeList->prev;
	freeList->prev->next = node;
	freeList->prev = node;
	printf("From Insert\n");
	printFreeList();
}

Node *popFreeList() {
	if (freeList == NULL) {
		return NULL;
	}

	Node *toReturn = freeList;
	if (freeList->next == freeList) {
		freeList = NULL;
		return toReturn;
	}
	else {
		freeList->prev->next = freeList->next;
		freeList->next->prev = freeList->prev;
		freeList = freeList->next;
	}
	toReturn->next = NULL;
	toReturn->prev = NULL;
	return toReturn;
}

void freeListRemove(Node* node) {
    Node* workingNode = freeList;
	if (freeList != NULL && node == freeList && freeList->next == freeList) {
		freeList = NULL;
		node->next = NULL;
		node->prev = NULL;
		return ;
	}
    while(workingNode != NULL) {
		if (node == workingNode) {
			if (freeList == node) {
				freeList = node->next;
			}
			node->next->prev = node->prev;
			node->prev->next = node->next;
			node->next = NULL;
			node->prev = NULL;
			break;
		}
		workingNode = workingNode->next;
		if (workingNode == freeList) {
			break;
		}
	}
	printf("From Remove ");
	printFreeList();
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

