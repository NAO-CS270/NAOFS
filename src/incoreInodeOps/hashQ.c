#include "../incoreInodeOps/hashQ.h"

static const size_t inodeHashSize = INODE_HASH_SIZE;

static Node* hashQ[inodeHashSize];

// returns the index of the inode in hashQ
size_t getHash(size_t deviceNumber, size_t inodeNumber) {
    return (deviceNumber + inodeNumber << 6 + inodeNumber << 16 - inodeNumber) % inodeHashSize;
}

// insert into the hash table
void insertInHash(Node* node) {
    inCoreiNode* inode = node->inode;
    size_t index = getHash(inode->device_number, inode->inode_number);
    Node* head = hashQ[index];

    // change the hash pointers
    node->hash_prev = NULL;
    node->hash_next = head;
    head->hash_prev = node;
    hashQ[index] = node;
}

// returns the inode cached in the hash queue, NULL if not found
Node* hashLookup(size_t deviceNumber, size_t inodeNumber) {
    size_t hash_number = getHash(deviceNumber, inodeNumber);
    Node* head = hashQ[hash_number];
    if(NULL == head) {
        return NULL;
    }
    while(head != NULL) {
        if(head->inode->inode_number == inodeNumber) {
            return head;
        }
        head = head->hash_next;
    }
    return NULL;
}

