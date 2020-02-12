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
    Node* head = freeList;
    // Since we are using a pointer, can't we delete it right away?
    while(NULL != head) {
        if(head->inode->inode_number == node->inode->inode_number) {
            // if tail node has to be removed
            if(NULL == head->next) {
                head->prev->next = NULL;
            } // if head has to be removed
            else if(NULL == head->prev) {
                head->next->prev = NULL;
                Node* temp = head->next;
                head->next = NULL;
                head->prev = NULL;
                head = temp;
                return;
            } else {
                head->next->prev = head->prev;
                head->prev->next = head->next;
            }
            head->next = NULL;
            head->prev = NULL;
            return;
        }
        head = head->next;
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

