/* Merge Sort algorithm implementation on linked list with recursive calls
 * Original code from http://www.sanfoundry.com/c-program-implement-merge-sort-linked-list/
 *
 * the minked list is considered already loaded in memory with first node at TST_BASE address
 *
 * TDk @ CITI Lab 2017
*/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "address_map.h"
#include "hal.h"
#include "libdycton.h"

#define ALLOC_TEST_SIZE    (sizeof(uint32_t))

struct node
{
    int data;
    struct node* next;
};

struct node* sorted_merge(struct node* a, struct node* b);
void front_back_split(struct node* source, struct node** frontRef, struct node** backRef);


void merge_sort(struct node** headRef)
{
    struct node* head = *headRef;
    struct node* a;
    struct node* b;
    if ((head == NULL) || (head -> next == NULL))
    {
        return;
    }
    front_back_split(head, &a, &b);
    merge_sort(&a);
    merge_sort(&b);
    *headRef = sorted_merge(a, b);
}


struct node* sorted_merge(struct node* a, struct node* b)
{
    struct node* result = NULL;

    if (a == NULL)
        return (b);
    else if (b == NULL)
        return (a);

    if ( a-> data <= b -> data)
    {
        result = a;
        result -> next = sorted_merge(a -> next, b);
    }
    else
    {
        result = b;
        result -> next = sorted_merge(a, b -> next);
    }
    return (result);
}



void front_back_split(struct node* source, struct node** frontRef, struct node** backRef)
{
    struct node* fast;
    struct node* slow;
    if (source == NULL || source->next == NULL)
    {
        *frontRef = source;
        *backRef = NULL;
    }
    else
    {
        slow = source;
        fast = source -> next;
        while (fast != NULL)
        {
            fast = fast -> next;
            if (fast != NULL)
            {
                slow = slow -> next;
                fast = fast -> next;
            }
        }

        *frontRef = source;
        *backRef = slow -> next;
        slow -> next = NULL;
    }
}


void print_list(struct node *node)
{
    while (node != NULL)
    {
        printf("%u\n",(unsigned int)node->data);
        node = node -> next;
    }
}


void push(struct node** head_ref, int new_data)
{
    struct node* new_node = (struct node*) malloc(sizeof(struct node));
    new_node -> data  = new_data;
    new_node->next = (*head_ref);
    (*head_ref) = new_node;
}


int main(void)
{
    printf("\n\n\n================== MergeSort for DYCTON ==================\n");
    printf("(TDk @ CITI Lab)\n\n");

    printf("base address of linked list : 0x%x \n",(unsigned int)TST_BASE);

    struct node* a = (struct node*)TST_BASE;

    // print_list(a);

    printf("sorting linked list...\n");
    merge_sort(&a);
    printf("sorting done.");

    // print_list(a);

    printf("\n=========================== THE END ===========================\n");
    exit(0);

    return 0;

}