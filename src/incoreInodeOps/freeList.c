#include <stdlib.h>
#include "incoreInodeOps/freeList.h"

static Node* freeList;

void initFreeInCoreINodeList() {
    freeList = NULL;
    int i;
    for(i = 0; i < INODE_BUFFER_SIZE; i++) {
        Node* node = (Node*)malloc(sizeof(struct Node));
        node->inode = (inCoreiNode*)malloc(sizeof(inCoreiNode));
        iNode* disk_inode = (iNode*)malloc(sizeof(iNode));
        insertDiskInodeData(disk_inode, node->inode);
        freeListInsert(node);
    }
}

void freeListInsert(Node* node) {
    Node* head = freeList;
    if (head == NULL)
        head = node;
    node->next = head;
    head->prev = node;
    node->prev = NULL;
    head = node;
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

