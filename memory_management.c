#include <stdio.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE 0
typedef unsigned int uint; 
typedef int BOOLEAN;

struct memorySegment {
    uint startAddress, length;
    BOOLEAN occupied;
    struct memorySegment* next;
};

/* ==================== (1) LIST FUNCTIONS */

void printList(struct memorySegment* memList) {
    struct memorySegment* current = memList;
    while(current != NULL){
        printf("%d %d %s\n", current->startAddress, current->length, current->occupied ? "Occupied!" : "Free");
        current = current->next;
    }
}

void insertListItemAfter(struct memorySegment** current) {
    struct memorySegment* newItem = (struct memorySegment*)malloc(sizeof(struct memorySegment));
    if(*current != NULL){ 
        struct memorySegment* oldCurrentNext = (*current)->next;
        (*current)->next = newItem;
        newItem->next = oldCurrentNext;
    }
    else {
        (*current) = newItem;
        (*current)->next = NULL;
    }
}

void removeListItemAfter(struct memorySegment* current) {
    if(current != NULL){
        struct memorySegment* oldCurrentNext = current->next;
        if(oldCurrentNext != NULL){
            current->next = oldCurrentNext->next;
            free(oldCurrentNext);
        }
    }
}

/* ==================== (2) FIXED MEMORY ALLOCATIONS */

struct memorySegment* assignFirst(struct memorySegment* memList, uint requestedMem){
    struct memorySegment* current = memList;
    while(current){
        BOOLEAN free = !current->occupied;
        BOOLEAN sufficientMemory = current->length >= requestedMem;
        if(free && sufficientMemory){
            current->occupied = TRUE;
            return current;
        }
        current = current->next;
    }
    return NULL;
}

struct memorySegment* assignBest(struct memorySegment* memList, uint requestedMem){
    struct memorySegment* current = memList;
    struct memorySegment* bestFit = NULL;
    uint bestFitMemoryLength = __UINT32_MAX__;
    while(current != NULL){
        BOOLEAN free = !current->occupied;    
        BOOLEAN sufficientMemory = current->length >= requestedMem;
        BOOLEAN fitsBetter = sufficientMemory && current->length < bestFitMemoryLength;
        if(free && fitsBetter){
            bestFit = current;
            bestFitMemoryLength = current->length;
        }
        current = current->next;
    }
    if(bestFit != NULL)
        bestFit->occupied = TRUE;
    return bestFit;
}

struct memorySegment* assignNext(struct memorySegment* memList, uint requestedMem){
    static struct memorySegment* current = NULL;
    if(current == NULL) current = memList; // exexutes only the first time the routine is called!

    struct memorySegment* currentStartingPos = current;
    do{
        BOOLEAN free = !current->occupied;
        BOOLEAN sufficientMemory = current->length >= requestedMem;
        if(free && sufficientMemory){
            current->occupied = TRUE;
            return current;
        }
        current = current->next;
        if(current == NULL) current = memList;
    }while(current != currentStartingPos);
    
    return NULL;
}

void reclaim(struct memorySegment* thisOne){
    thisOne->occupied = FALSE;
}

/* ==================== (3) DYNAMIC MEMORY ALLOCATIONS */

void closeMemoryHoles(struct memorySegment* node, uint requestedMem){
    struct memorySegment* nextNode = node->next;

    BOOLEAN NodeExtraMemory = node->length > requestedMem;
    if(nextNode){
        BOOLEAN nextNodeOccupied = nextNode->occupied;
        if(NodeExtraMemory && nextNodeOccupied){
            insertListItemAfter(&node);
            struct memorySegment* newNextNode = node->next;
            newNextNode->occupied = FALSE;
            newNextNode->startAddress = node->startAddress + requestedMem;
            newNextNode->length = node->length - requestedMem;
            node->length = requestedMem;
        }
        else if(NodeExtraMemory && !nextNodeOccupied){
            nextNode->startAddress = node->startAddress + requestedMem;
            nextNode->length = nextNode->length + node->length - requestedMem;
            node->length = requestedMem;
        }
    }
    else if(nextNode == NULL){
        if(NodeExtraMemory){
            insertListItemAfter(&node);
            struct memorySegment* newNextNode = node->next;
            newNextNode->occupied = FALSE;
            newNextNode->startAddress = node->startAddress + requestedMem;
            newNextNode->length = node->length - requestedMem;
            node->length = requestedMem;
        }
    }

    return;
}

struct memorySegment* assignFirstDyn(struct memorySegment* memList, uint requestedMem){
    struct memorySegment* firstNode = assignFirst(memList, requestedMem);
    if(firstNode == NULL)
        return NULL;
    
    closeMemoryHoles(firstNode, requestedMem);

    return firstNode;
}

struct memorySegment* assignBestDyn(struct memorySegment* memList, uint requestedMem){
    struct memorySegment* bestNode = assignBest(memList, requestedMem);
    if(bestNode == NULL)
        return NULL;

    closeMemoryHoles(bestNode, requestedMem);
    
    return bestNode;
}

struct memorySegment* assignNextDyn(struct memorySegment* memList, uint requestedMem){
    struct memorySegment* nextNode = assignBest(memList, requestedMem);
    if(nextNode == NULL)
        return NULL;

    closeMemoryHoles(nextNode, requestedMem);
    
    return nextNode;
}

void reclaimDyn(struct memorySegment* thisOne){
    reclaim(thisOne);
    struct memorySegment* nextNode = thisOne->next;
    if(nextNode == NULL)
        return;

    BOOLEAN nextNodeFree = !nextNode->occupied;
    if(nextNodeFree){
        thisOne->length = thisOne->length + nextNode->length;
        thisOne->next = nextNode->next;
        free(nextNode);
    }

    return;
}