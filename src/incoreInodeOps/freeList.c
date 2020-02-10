#include <stdlib.h>
#include "./freeList.h"

void initFreeInCoreINodeList() {
    freeList = NULL;
    for(int i = 0; i < INODE_BUFFER_SIZE; i++) {
        Node* node = (Node*)malloc(sizeof(struct Node));
        node->inode = (inCoreiNode*)malloc(sizeof(inCoreiNode));
        node->inode->disk_iNode = (iNode*)malloc(sizeof(iNode));
        freeListInsert(node);
    }
}

void freeListInsert(Node* node) {
    Node* head = freeList;
    if (head = NULL)
        head = node;
    node->next = head;
    head->prev = node;
    node->prev = NULL;
    head = node;
}

void freeListRemove(Node* node) {
    Node* head = freeList;
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
