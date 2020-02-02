#include <stdlib.h>
#include "inodecache/freeList.h"
#include "incoreInodeOps/node.h"

void freeList_insert(Node* head, Node* node) {
    node->next = head;
    head->prev = node;
    node->prev = NULL;
    head = node;
}

void freeList_remove(Node* head, Node* node) {
    while(NULL != head) {
        if(head->inode->inode_number == node->inode->inode_number) {
            if(NULL == head->next) {
                head->prev->next = NULL;
            } else {
                head->next->prev = head->prev;
            }
            if(NULL == head->prev) {
                head->next->prev = NULL;
                return
            } else {
                head->prev->next = head->next;
            }
            return
        }
        head = head->next;
    }
}


