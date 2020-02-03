#include <stdlib.h>
#include "incoreInodeOps/freeList.h"

void freeListInsert(Node* head, Node* node) {
    node->next = head;
    head->prev = node;
    node->prev = NULL;
    head = node;
}

void freeListRemove(Node* head, Node* node) {
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


