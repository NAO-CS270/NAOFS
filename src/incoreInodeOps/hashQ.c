#include "mandsk/params.h"
#include "incoreInodeOps/hashQ.h"
#include "incoreInodeOps/node.h"

static const size_t inodeHashSize = INODE_HASH_SIZE;

size_t get_hash(size_t device_number, size_t inode_number) {
    return (device_number + inode_number) % inodeHashSize;
}

// insert into the hash table
void insert(Node* node, Node** hashQ) {
    inCoreiNode* inode = node->inode;
    size_t index = get_hash(inode->device_number, inode->inode_number);
    Node* head = hashQ[index];

    // change the hash pointers
    node->hash_prev = NULL;
    node->hash_next = head;
    head->hash_prev = node;

//    // remove from free list
//    // check if node is free list's head
//    if(NULL == node->prev) {
//        node->next->prev = NULL;
//
//    }
    // TODO: Similarly in freelist.
}

/**
 *
 */
Node* hash_lookup(size_t deviceNumber, size_t inodeNumber, Node** hashQ) {
    size_t hash_number = get_hash(deviceNumber, iNodeNumber);
    Node* head = hashQ[hash_number];
    if(NULL == head) {
        return NULL;
    }
    while(head != NULL) {
        if(head->inode->inode_number == iNodeNumber) {
            return head;
        }
        head = head->hash_next;
    }
    return NULL;
}

