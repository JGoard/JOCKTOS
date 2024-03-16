/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define false 0
#define true 1
typedef struct DataStructure DataStructure;
struct DataStructure {
char* name;
uint32_t data;
uint32_t priority;
DataStructure* next;
};

typedef struct LinkedListStatus {
    _Bool removeFailed;
    _Bool updateFailed;
    _Bool findFailed;
} LinkedListStatus;

LinkedListStatus status;

void insertDataStructure(DataStructure** head, DataStructure* newNode) {

    if (*head == NULL || newNode->priority >= (*head)->priority) {
        newNode->next = *head;
        *head = newNode;
    } else {
        DataStructure* current = *head;
        while (current->next != NULL && current->next->priority > newNode->priority) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

void removeDataStructure(DataStructure** head, DataStructure* nodeToRemove) {
    status.removeFailed = false;

    if (*head == NULL || nodeToRemove == NULL) {
        status.removeFailed = true;
        return;
    }

    if (*head == nodeToRemove) {
        *head = nodeToRemove->next;
    } else {
        DataStructure* current = *head;
        while (current->next != NULL && current->next != nodeToRemove) {
            current = current->next;
        }
        if (current->next == nodeToRemove) {
            current->next = nodeToRemove->next;
        } else {
            status.removeFailed = true;
        }
    }
}

void updateDataStructurePriority(DataStructure** head, DataStructure* nodeToUpdate, uint32_t newPriority) {
    status.updateFailed = false;

    if (*head == NULL || nodeToUpdate == NULL) {
        status.updateFailed = true;
        return;
    }

    // Remove the node from the list
    removeDataStructure(head, nodeToUpdate);

    // Update the priority of the node
    nodeToUpdate->priority = newPriority;

    // Reinsert the node into the list at its new position
    insertDataStructure(head, nodeToUpdate);
}

DataStructure* findDataStructureByName(DataStructure* head, const char* name) {
    status.findFailed = false;

    DataStructure* current = head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    status.findFailed = true;
    return NULL;
}
void printDataStructureList(DataStructure* head) {
    printf("List Status:\n");
    printf("Remove Failed: %s\n", status.removeFailed ? "true" : "false");
    printf("Update Failed: %s\n", status.updateFailed ? "true" : "false");
    printf("Find Failed: %s\n", status.findFailed ? "true" : "false");

    printf("\nList Contents:\n");
    DataStructure* current = head;
    while (current != NULL) {
        printf("Name: %s, Priority: %u\n", current->name, current->priority);
        current = current->next;
    }
}
int main()
{
    DataStructure dts = {"head", 0, 1, NULL};
    DataStructure* head = &dts;
    insertDataStructure(&head, "swamp", 1, 1);
    insertDataStructure(&head, "moose", 2, 2);
    insertDataStructure(&head, "lagoon", 3, 1);
    printDataStructureList(head);
    updateDataStructurePriority(&head, head->next->next, 10);
    printf("updated\n");
    printDataStructureList(head);
    DataStructure* swamp = findDataStructureByName(head, "swamp");
    removeDataStructure(&head, swamp);
    printf("removed\n");
    printDataStructureList(head);
    return 0;
}
