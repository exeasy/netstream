#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include "utils.h"
#include "list.h"

void cr_list_add(struct cr_list_head* new, struct cr_list_head *head)
{
	struct cr_list_head* next = head->next;
	head->next = new;
	new->next = next;
	new->prev = head;
	next->prev = new;
}
void cr_list_add_tail(struct cr_list_head* new, struct cr_list_head *head)
{
	struct cr_list_head* prev = head->prev;
	head->prev = new;
	new->prev = prev;
	new->next = head;
	prev->next = new;
}
void cr_list_add_in_order(struct cr_list_head* new, struct cr_list_head* head, int key, fetch_keyword v)
{
	int key1;
	struct cr_list_head *ptr;
	ptr = head;
	do{
		ptr = ptr->next;
		key1 = (*v)(ptr);
	}while(key>key1 && ptr != head);
	cr_list_add_tail(new, ptr);
}
int cr_list_empty(const struct cr_list_head* head)
{
	if(head->next == head || head->prev == head)
		return TRUE;
	return FALSE;
}
int cr_list_del(struct cr_list_head* entry)
{
	if(entry->next == entry || entry->prev == entry)
		return CONTAINER_EMPTY_ERR;
	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;
	entry->next = 0;
	entry->prev = 0;
	return NO_ERR;
}
struct stack_item
{
	void* item;
	struct cr_list_head ptrs;
};
void cr_stack_push(void* item, struct cr_list_head* stack)
{
	assert(item != NULL);
	assert(stack != NULL);
	struct stack_item* stack_item = malloc_z(struct stack_item);
	stack_item->item = item;
	cr_list_add(&stack_item->ptrs, stack);
}
void* cr_stack_pop(struct cr_list_head* stack)
{
	assert(stack != NULL);
	struct cr_list_head* item = stack->next;
	void* result;
	if(cr_list_del(item) == NO_ERR)
	{
		struct stack_item* stack_item = CR_LIST_ENTRY(item, struct stack_item, ptrs);
		result = stack_item->item;
		free(stack_item);
		return result;
	}
	else
		return NULL;
}
